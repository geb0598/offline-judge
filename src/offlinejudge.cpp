#include "offlinejudge.h"

#include <iostream>
#include <string>
#include <sstream>
#include <utility>

JudgeResult::JudgeResult(size_t id, Status status) : id_(id), status_(status) {}

JudgeResult::JudgeResult(
    size_t id,
    std::vector<LineStatus>&& line_status_vec,
    std::chrono::system_clock::duration elapsed_time
) : id_(id), line_status_vec_(std::move(line_status_vec)), elapsed_time_(elapsed_time) {
    JudgeResult::Status status = JudgeResult::Status::JR_CORRECT;
    for (auto line_status : line_status_vec_) {
        if (status == JudgeResult::Status::JR_CORRECT) {
            if (line_status == JudgeResult::LineStatus::JRL_WRONG) {
                status = JudgeResult::Status::JR_WRONG;
            } else if (line_status == JudgeResult::LineStatus::JRL_ILL_FORMED) {
                status = JudgeResult::Status::JR_ILL_FORMED;
            }
        } else if (status == JudgeResult::Status::JR_WRONG) {
            if (line_status == JudgeResult::LineStatus::JRL_ILL_FORMED) {
                status = JudgeResult::Status::JR_ILL_FORMED;
            }
        } else if (status == JudgeResult::Status::JR_ILL_FORMED) {
            continue;
        }
    }
    status_ = status;
}

size_t JudgeResult::get_id() const {
    return id_;
}

size_t JudgeResult::get_line_count() const {
    return line_status_vec_.size();
}

JudgeResult::Status JudgeResult::get_status() const {
    return status_;
}

JudgeResult::LineStatus JudgeResult::get_line_status(size_t index) const {
    return line_status_vec_[index];
}

std::chrono::system_clock::duration JudgeResult::get_elapsed_time() const {
    return elapsed_time_;
}

const OfflineJudge& OfflineJudge::GetOfflineJudge() {
    static OfflineJudge offline_judge;
    return offline_judge;
}

JudgeResult OfflineJudge::Judge(size_t id, const std::string& user_answer, const std::string& example_answer) const {
    auto start_time = std::chrono::system_clock::now();
    std::istringstream user_stream(user_answer), example_stream(example_answer);
    std::vector<JudgeResult::LineStatus> line_status_vec;
    while (true) {
        if (user_stream.eof() && example_stream.eof()) {
            break;
        }
        std::string user_line, example_line;
        if (!user_stream.eof()) {
            std::getline(user_stream, user_line);
        }
        if (!example_stream.eof()) {
            std::getline(example_stream, example_line);
        }
       line_status_vec.push_back(CompareLine(user_line, example_line)); 
    }
    auto elapsed_time = std::chrono::system_clock::now() - start_time; 
    return JudgeResult(id, std::move(line_status_vec), elapsed_time);
}

JudgeResult::LineStatus OfflineJudge::CompareLine(const std::string& user_line, const std::string& example_line) const {
    std::istringstream user_stream(user_line), example_stream(example_line);
    std::vector<std::string> user_tokens, example_tokens;
    std::string buf;
    while (user_stream >> buf) {
        user_tokens.push_back(buf);
    }
    while (example_stream >> buf) {
        example_tokens.push_back(buf);
    }
    if (user_tokens.size() != example_tokens.size()) {
        return JudgeResult::LineStatus::JRL_ILL_FORMED;
    }
    for (size_t i = 0; i < user_tokens.size(); ++i) {
        if (user_tokens[i] != example_tokens[i]) {
            return JudgeResult::LineStatus::JRL_WRONG;
        }
    } 
    return JudgeResult::LineStatus::JRL_CORRECT;
}