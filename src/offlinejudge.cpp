#include "offlinejudge.h"

#include <iostream>
#include <string>
#include <sstream>
#include <utility>

OfflineJudge::JudgeResult::JudgeResult(const std::vector<JudgeResult::LineStatus>& line_stats, double elapsed_time) : line_stats_(line_stats), elapsed_time_(elapsed_time) {}

OfflineJudge::JudgeResult::JudgeResult(std::vector<JudgeResult::LineStatus>&& line_stats, double elapsed_time) : line_stats_(std::move(line_stats)), elapsed_time_(elapsed_time) {}

const std::vector<OfflineJudge::JudgeResult::LineStatus>& OfflineJudge::JudgeResult::getLineStats() const {
    return line_stats_;
}

const double OfflineJudge::JudgeResult::getElapsedTime() const {
    return elapsed_time_;
}

const OfflineJudge& OfflineJudge::getOfflineJudge() {
    static OfflineJudge offline_judge;
    return offline_judge;
}

OfflineJudge::JudgeResult OfflineJudge::judge(const std::string& user_answer, const std::string& example_answer) const {
    std::istringstream user_stream(user_answer), example_stream(example_answer);
    auto start_time = std::chrono::system_clock::now();
    std::vector<OfflineJudge::JudgeResult::LineStatus> result;
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
        result.push_back(compareLine(user_line, example_line)); 
    }
    double elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start_time).count();
    return OfflineJudge::JudgeResult(std::move(result), elapsed_time);
}

OfflineJudge::JudgeResult::LineStatus OfflineJudge::compareLine(const std::string& user_line, const std::string& example_line) const {
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
        return OfflineJudge::JudgeResult::LineStatus::ILL_FORMED;
    }
    for (size_t i = 0; i < user_tokens.size(); ++i) {
        if (user_tokens[i] != example_tokens[i]) {
            return OfflineJudge::JudgeResult::LineStatus::WRONG;
        }
    } 
    return OfflineJudge::JudgeResult::LineStatus::CORRECT;
}