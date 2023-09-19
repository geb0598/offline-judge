#pragma once

#include "solution.h"

#include <vector>
#include <string>

class OfflineJudge {
public:
    class JudgeResult {
    public:
        enum class LineStatus {
            CORRECT,
            WRONG,
            ILL_FORMED
        };
        
        JudgeResult(const std::vector<LineStatus>& line_stats, double elapsed_time);
        JudgeResult(std::vector<LineStatus>&& line_stats, double elapsed_time);
        const std::vector<LineStatus>& getLineStats() const;
        const double getElapsedTime() const;
    private:
        std::vector<LineStatus> line_stats_;
        double elapsed_time_;
    };
    
    static const OfflineJudge& getOfflineJudge();
    JudgeResult judge(const std::string& user_answer, const std::string& example_answer) const;

private:
    OfflineJudge() {};
    OfflineJudge(const OfflineJudge& offline_judge) {};
    ~OfflineJudge() {};
    JudgeResult::LineStatus compareLine(const std::string& user_line, const std::string& example_line) const;
};