#ifndef JUDGE_RESULT_H
#define JUDGE_RESULT_H

#include <chrono>
#include <filesystem>
#include <ostream>
#include <string>
#include <vector>

#include <sys/resource.h>

#include "renderer.h"
#include "labeler.h"

#include "result.h"

namespace oj {

struct TokenJudgeData;
struct LineJudgeData;

class JudgeResult : public Result {
public:
    virtual ~JudgeResult() = default;

    JudgeResult (
        const std::string&                 user_answer,
        const std::string&                 correct_answer,
        const std::vector<TokenJudgeData>& token_data,
        const std::vector<LineJudgeData>&  line_data
    );
    JudgeResult(const JudgeResult& other) = default;
    JudgeResult(JudgeResult&& other) noexcept = default;

    JudgeResult& operator=(const JudgeResult& other) = default;
    JudgeResult& operator=(JudgeResult&& other) noexcept = default;

    virtual void        Render(std::ostream& os, const Renderer& renderer) const = 0;
    virtual std::string Label(const Labeler& labeler) const override;

    virtual bool        is_success() const = 0;
            std::string user_answer() const;
            std::string correct_answer() const;

private:
    std::string                 user_answer_;
    std::string                 correct_answer_;
    std::vector<TokenJudgeData> token_data_;
    std::vector<LineJudgeData>  line_data_;
};

class JudgeSuccess : public JudgeResult {
public:
    virtual ~JudgeSuccess() = default;

    JudgeSuccess (
        const std::string&                 user_answer,
        const std::string&                 correct_answer,
        const std::vector<TokenJudgeData>& token_data,
        const std::vector<LineJudgeData>&  line_data
    );
    JudgeSuccess(const JudgeSuccess& other) = default;
    JudgeSuccess(JudgeSuccess&& other) noexcept = default;

    JudgeSuccess& operator=(const JudgeSuccess& other) = default;
    JudgeSuccess& operator=(JudgeSuccess&& other) noexcept = default;

    virtual void        Render(std::ostream& os, const Renderer& renderer) const = 0;
    virtual std::string Label(const Labeler& labeler) const override;

    virtual bool        is_success() const = 0;
};

class JudgeFailure : public JudgeResult {
public:
    virtual ~JudgeFailure() = default;

    JudgeFailure (
        const std::string&                 user_answer,
        const std::string&                 correct_answer,
        const std::vector<TokenJudgeData>& token_data,
        const std::vector<LineJudgeData>&  line_data
    );
    JudgeFailure(const JudgeFailure& other) = default;
    JudgeFailure(JudgeFailure&& other) noexcept = default;

    JudgeFailure& operator=(const JudgeFailure& other) = default;
    JudgeFailure& operator=(JudgeFailure&& other) noexcept = default;

    virtual void        Render(std::ostream& os, const Renderer& renderer) const = 0;
    virtual std::string Label(const Labeler& labeler) const override;

    virtual bool        is_success() const = 0;
};

class JudgeFailureInvalidOutputFormat : public JudgeFailure {
public:
    virtual ~JudgeFailureInvalidOutputFormat() = default;

    JudgeFailureInvalidOutputFormat (
        const std::string&                 user_answer,
        const std::string&                 correct_answer,
        const std::vector<TokenJudgeData>& token_data,
        const std::vector<LineJudgeData>&  line_data
    );
    JudgeFailureInvalidOutputFormat(const JudgeFailureInvalidOutputFormat& other) = default;
    JudgeFailureInvalidOutputFormat(JudgeFailureInvalidOutputFormat&& other) noexcept = default;

    JudgeFailureInvalidOutputFormat& operator=(const JudgeFailureInvalidOutputFormat& other) = default;
    JudgeFailureInvalidOutputFormat& operator=(JudgeFailureInvalidOutputFormat&& other) noexcept = default;

    virtual void        Render(std::ostream& os, const Renderer& renderer) const = 0;
    virtual std::string Label(const Labeler& labeler) const override;

    virtual bool        is_success() const = 0;
};

class JudgeFailureOutputExceeded : public JudgeFailure {
public:
    virtual ~JudgeFailureOutputExceeded() = default;

    JudgeFailureOutputExceeded (
        const std::string&                 user_answer,
        const std::string&                 correct_answer,
        const std::vector<TokenJudgeData>& token_data,
        const std::vector<LineJudgeData>&  line_data
    );
    JudgeFailureOutputExceeded(const JudgeFailureOutputExceeded& other) = default;
    JudgeFailureOutputExceeded(JudgeFailureOutputExceeded&& other) noexcept = default;

    JudgeFailureOutputExceeded& operator=(const JudgeFailureOutputExceeded& other) = default;
    JudgeFailureOutputExceeded& operator=(JudgeFailureOutputExceeded&& other) noexcept = default;

    virtual void        Render(std::ostream& os, const Renderer& renderer) const = 0;
    virtual std::string Label(const Labeler& labeler) const = 0;

    virtual bool        is_success() const override;
};

std::shared_ptr<JudgeResult> CreateJudgeResult (
    int                                status,
    const std::string&                 user_answer,
    const std::string&                 correct_answer,
    const std::vector<TokenJudgeData>& token_data,
    const std::vector<LineJudgeData>&  line_data
);

}

#endif