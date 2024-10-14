#ifndef SUBMISSION_RESULT_H
#define SUBMISSION_RESULT_H

#include <filesystem>
#include <ostream>
#include <string>
#include <vector>

#include "renderer.h"
#include "labeler.h"

#include "result.h"
#include "compilation_result.h"
#include "execution_result.h"
#include "judge_result.h"

namespace oj {

class SubmissionResult : public Result {
public:
    virtual ~SubmissionResult() = default;

    SubmissionResult (
        const std::shared_ptr<CompilationResult>&            compilation_result,
        const std::vector<std::shared_ptr<ExecutionResult>>& execution_results,
        const std::vector<std::shared_ptr<JudgeResult>>&     judge_results 
    );
    SubmissionResult(const SubmissionResult& other) = default;
    SubmissionResult(SubmissionResult&& other) noexcept = default;

    SubmissionResult& operator=(const SubmissionResult& other) = default;
    SubmissionResult& operator=(SubmissionResult&& other) noexcept = default;

    virtual void          Render(std::ostream& os, const Renderer& renderer) const = 0;
    virtual std::string   Label(const Labeler& labeler) const = 0;

    virtual bool          is_success() const = 0;
    std::filesystem::path source() const;
    std::filesystem::path program() const;

private:
    std::shared_ptr<CompilationResult>            compilation_result_;
    std::vector<std::shared_ptr<ExecutionResult>> execution_results_;
    std::vector<std::shared_ptr<JudgeResult>>     judge_results_;
};

class SubmissionSuccess : public SubmissionResult {
    virtual ~SubmissionSuccess() = default;

    SubmissionSuccess (
        const std::shared_ptr<CompilationSuccess>&            compilation_result,
        const std::vector<std::shared_ptr<ExecutionSuccess>>& execution_results,
        const std::vector<std::shared_ptr<JudgeSuccess>>&     judge_results 
    );
    SubmissionSuccess(const SubmissionSuccess& other) = default;
    SubmissionSuccess(SubmissionSuccess&& other) noexcept = default;

    SubmissionSuccess& operator=(const SubmissionSuccess& other) = default;
    SubmissionSuccess& operator=(SubmissionSuccess&& other) noexcept = default;

    virtual void        Render(std::ostream& os, const Renderer& renderer) const = 0;
    virtual std::string Label(const Labeler& labeler) const = 0;

    virtual bool        is_success() const override;
};

class SubmissionFailure : public SubmissionResult {
    virtual ~SubmissionFailure() = default;

    SubmissionFailure (
        const std::shared_ptr<CompilationResult>&            compilation_result,
        const std::vector<std::shared_ptr<ExecutionResult>>& execution_results,
        const std::vector<std::shared_ptr<JudgeResult>>&     judge_results 
    );
    SubmissionFailure(const SubmissionFailure& other) = default;
    SubmissionFailure(SubmissionFailure&& other) noexcept = default;

    SubmissionFailure& operator=(const SubmissionFailure& other) = default;
    SubmissionFailure& operator=(SubmissionFailure&& other) noexcept = default;

    virtual void        Render(std::ostream& os, const Renderer& renderer) const = 0;
    virtual std::string Label(const Labeler& labeler) const = 0;

    virtual bool        is_success() const override;
};

std::shared_ptr<SubmissionResult> CreateSubmissionResult (
    const std::shared_ptr<CompilationResult>&            compilation_result,
    const std::vector<std::shared_ptr<ExecutionResult>>& execution_results,
    const std::vector<std::shared_ptr<JudgeResult>>&     judge_results 
);

}

#endif