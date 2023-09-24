#pragma once

#include "solution.h"

#include <filesystem>
#include <vector>
#include <string>

class JudgeResult {
public:
    enum class Status {
        JR_CORRECT,
        JR_WRONG,
        JR_ILL_FORMED,
        JR_COMPILE_ERROR,
        JR_TIMEOUT,
        JR_OUT_OF_MEMORY,
        JR_RUNTIME_ERROR
    };

    enum class LineStatus {
        JRL_CORRECT,
        JRL_WRONG,
        JRL_ILL_FORMED
    };

    JudgeResult(size_t id, Status status);

    JudgeResult(
        size_t id, 
        std::vector<LineStatus>&& line_status_vec, 
        std::chrono::system_clock::duration elapsed_time);
    
    size_t get_id() const;
    size_t get_line_count() const;
    Status get_status() const;
    LineStatus get_line_status(size_t index) const;
    std::chrono::system_clock::duration get_elapsed_time() const;

private:
    size_t id_;
    Status status_;
    std::vector<LineStatus> line_status_vec_;
    std::chrono::system_clock::duration elapsed_time_;
};

class OfflineJudge {
public:
    static const OfflineJudge& GetOfflineJudge();
    JudgeResult Judge(size_t id, const std::filesystem::path& user_path, const std::filesystem::path& example_path) const;
    JudgeResult Judge(size_t id, const std::string& user_answer, const std::string& example_answer) const;

private:
    OfflineJudge() {};
    OfflineJudge(const OfflineJudge& offline_judge) {};
    ~OfflineJudge() {};
    JudgeResult::LineStatus CompareLine(const std::string& user_line, const std::string& example_line) const;
};