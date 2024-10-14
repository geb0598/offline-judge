#ifndef EXECUTION_RESULT_H
#define EXECUTION_RESULT_H

#include <filesystem>
#include <ostream>
#include <string>
#include <vector>

#include <sys/resource.h>

#include "renderer.h"
#include "labeler.h"

#include "result.h"

namespace oj {

class ExecutionResult : public Result {
public:
    virtual ~ExecutionResult() = default;

    ExecutionResult (
        const std::filesystem::path& program,
        const std::string&           input,
        const std::string&           output,
        const rusage&                usage
    );
    ExecutionResult(const ExecutionResult& other) = default;
    ExecutionResult(ExecutionResult&& other) noexcept = default;

    ExecutionResult& operator=(const ExecutionResult& other) = default;
    ExecutionResult& operator=(ExecutionResult&& other) noexcept = default;

    virtual void        Render(std::ostream& os, const Renderer& renderer) const = 0;
    virtual std::string Label(const Labeler& labeler) const override;

    virtual bool        is_success() const = 0;
            int         elapsed_time_sec() const;
            int         elapsed_time_usec() const;
            int         memory_usage() const;
            std::string input() const;
            std::string output() const;

private:
    std::filesystem::path program_;
    std::string           input_;
    std::string           output_;
    rusage                resource_usage_;
};

class ExecutionSuccess : public ExecutionResult {
public:
    virtual ~ExecutionSuccess() = default;

    ExecutionSuccess (
        const std::filesystem::path& program,
        const std::string&           input,
        const std::string&           output,
        const rusage&                usage
    );
    ExecutionSuccess(const ExecutionSuccess& other) = default;
    ExecutionSuccess(ExecutionSuccess&& other) noexcept = default;

    ExecutionSuccess& operator=(const ExecutionSuccess& other) = default;
    ExecutionSuccess& operator=(ExecutionSuccess&& other) noexcept = default;

    virtual void        Render(std::ostream& os, const Renderer& renderer) const override;
    virtual std::string Label(const Labeler& labeler) const override;

    virtual bool        is_success() const override;
};

class ExecutionFailure : public ExecutionResult {
public:
    virtual ~ExecutionFailure() = default;

    ExecutionFailure (
        const std::filesystem::path& program,
        const std::string&           input,
        const std::string&           output,
        const rusage&                usage
    );
    ExecutionFailure(const ExecutionFailure& other) = default;
    ExecutionFailure(ExecutionFailure&& other) noexcept = default;

    ExecutionFailure& operator=(const ExecutionFailure& other) = default;
    ExecutionFailure& operator=(ExecutionFailure&& other) noexcept = default;

    virtual void        Render(std::ostream& os, const Renderer& renderer) const = 0;
    virtual std::string Label(const Labeler& labeler) const = 0;

    virtual bool        is_success() const override;
};

class ExecutionFileNotExist : public ExecutionFailure {
public:
    virtual ~ExecutionFileNotExist() = default;

    ExecutionFileNotExist (
        const std::filesystem::path& program,
        const std::string&           input,
        const std::string&           output,
        const rusage&                usage
    );
    ExecutionFileNotExist(const ExecutionFileNotExist& other) = default;
    ExecutionFileNotExist(ExecutionFileNotExist&& other) noexcept = default;

    ExecutionFileNotExist& operator=(const ExecutionFileNotExist& other) = default;
    ExecutionFileNotExist& operator=(ExecutionFileNotExist&& other) noexcept = default;

    virtual void        Render(std::ostream& os, const Renderer& renderer) const = 0;
    virtual std::string Label(const Labeler& labeler) const = 0;

    virtual bool        is_success() const override;
};

class ExecutionFailureResourceUsage : public ExecutionFailure {
public:
    virtual ~ExecutionFailureResourceUsage() = default;

    ExecutionFailureResourceUsage(
        const std::filesystem::path& program,
        const std::string&           input,
        const std::string&           output,
        const rusage&                usage
    );
    ExecutionFailureResourceUsage(const ExecutionFailureResourceUsage& other) = default;
    ExecutionFailureResourceUsage(ExecutionFailureResourceUsage&& other) noexcept = default;

    ExecutionFailureResourceUsage& operator=(const ExecutionFailureResourceUsage& other) = default;
    ExecutionFailureResourceUsage& operator=(ExecutionFailureResourceUsage&& other) noexcept = default;

    virtual void        Render(std::ostream& os, const Renderer& renderer) const = 0;
    virtual std::string Label(const Labeler& labeler) const = 0;

    virtual bool        is_success() const override;
};

class ExecutionFailureTimeout : public ExecutionFailureResourceUsage {
public:
    virtual ~ExecutionFailureTimeout() = default;

    ExecutionFailureTimeout (
        const std::filesystem::path& program,
        const std::string&           input,
        const std::string&           output,
        const rusage&                usage
    );
    ExecutionFailureTimeout(const ExecutionFailureTimeout& other) = default;
    ExecutionFailureTimeout(ExecutionFailureTimeout&& other) noexcept = default;

    ExecutionFailureTimeout& operator=(const ExecutionFailureTimeout& other) = default;
    ExecutionFailureTimeout& operator=(ExecutionFailureTimeout&& other) noexcept = default;

    virtual void        Render(std::ostream& os, const Renderer& renderer) const = 0;
    virtual std::string Label(const Labeler& labeler) const = 0;

    virtual bool        is_success() const override;
};

class ExecutionFailureMemoryLimitExceeded : public ExecutionFailureResourceUsage {
public:
    virtual ~ExecutionFailureMemoryLimitExceeded() = default;

    ExecutionFailureMemoryLimitExceeded (
        const std::filesystem::path& program,
        const std::string&           input,
        const std::string&           output,
        const rusage&                usage
    );
    ExecutionFailureMemoryLimitExceeded(const ExecutionFailureMemoryLimitExceeded& other) = default;
    ExecutionFailureMemoryLimitExceeded(ExecutionFailureMemoryLimitExceeded&& other) noexcept = default;

    ExecutionFailureMemoryLimitExceeded& operator=(const ExecutionFailureMemoryLimitExceeded& other) = default;
    ExecutionFailureMemoryLimitExceeded& operator=(ExecutionFailureMemoryLimitExceeded&& other) noexcept = default;

    virtual void        Render(std::ostream& os, const Renderer& renderer) const = 0;
    virtual std::string Label(const Labeler& labeler) const = 0;

    virtual bool        is_success() const override;
};

class ExecutionFailureException : public ExecutionFailure {
public:
    virtual ~ExecutionFailureException() = default;

    ExecutionFailureException (
        const std::filesystem::path& program,
        const std::string&           input,
        const std::string&           output,
        const rusage&                usage
    );
    ExecutionFailureException(const ExecutionFailureException& other) = default;
    ExecutionFailureException(ExecutionFailureException&& other) noexcept = default;

    ExecutionFailureException& operator=(const ExecutionFailureException& other) = default;
    ExecutionFailureException& operator=(ExecutionFailureException&& other) noexcept = default;

    virtual void        Render(std::ostream& os, const Renderer& renderer) const = 0;
    virtual std::string Label(const Labeler& labeler) const = 0;

    virtual bool        is_success() const override;
};

class ExecutionFailureBadAlloc : public ExecutionFailureException {
public:
    virtual ~ExecutionFailureBadAlloc() = default;

    ExecutionFailureBadAlloc (
        const std::filesystem::path& program,
        const std::string&           input,
        const std::string&           output,
        const rusage&                usage
    );
    ExecutionFailureBadAlloc(const ExecutionFailureBadAlloc& other) = default;
    ExecutionFailureBadAlloc(ExecutionFailureBadAlloc&& other) noexcept = default;

    ExecutionFailureBadAlloc& operator=(const ExecutionFailureBadAlloc& other) = default;
    ExecutionFailureBadAlloc& operator=(ExecutionFailureBadAlloc&& other) noexcept = default;

    virtual void        Render(std::ostream& os, const Renderer& renderer) const = 0;
    virtual std::string Label(const Labeler& labeler) const = 0;

    virtual bool        is_success() const override;
};

class ExecutionFailureOutofRange : public ExecutionFailureException {
public:
    virtual ~ExecutionFailureOutofRange() = default;

    ExecutionFailureOutofRange (
        const std::filesystem::path& program,
        const std::string&           input,
        const std::string&           output,
        const rusage&                usage
    );
    ExecutionFailureOutofRange(const ExecutionFailureOutofRange& other) = default;
    ExecutionFailureOutofRange(ExecutionFailureOutofRange&& other) noexcept = default;

    ExecutionFailureOutofRange& operator=(const ExecutionFailureOutofRange& other) = default;
    ExecutionFailureOutofRange& operator=(ExecutionFailureOutofRange&& other) noexcept = default;

    virtual void        Render(std::ostream& os, const Renderer& renderer) const = 0;
    virtual std::string Label(const Labeler& labeler) const = 0;

    virtual bool        is_success() const override;
};

class ExecutionFailureLengthError : public ExecutionFailureException {
public:
    virtual ~ExecutionFailureLengthError() = default;

    ExecutionFailureLengthError (
        const std::filesystem::path& program,
        const std::string&           input,
        const std::string&           output,
        const rusage&                usage
    );
    ExecutionFailureLengthError(const ExecutionFailureLengthError& other) = default;
    ExecutionFailureLengthError(ExecutionFailureLengthError&& other) noexcept = default;

    ExecutionFailureLengthError& operator=(const ExecutionFailureLengthError& other) = default;
    ExecutionFailureLengthError& operator=(ExecutionFailureLengthError&& other) noexcept = default;

    virtual void        Render(std::ostream& os, const Renderer& renderer) const = 0;
    virtual std::string Label(const Labeler& labeler) const = 0;

    virtual bool        is_success() const override;
};

class ExecutionFailureInvalidArgument : public ExecutionFailureException {
public:
    virtual ~ExecutionFailureInvalidArgument() = default;

    ExecutionFailureInvalidArgument (
        const std::filesystem::path& program,
        const std::string&           input,
        const std::string&           output,
        const rusage&                usage
    );
    ExecutionFailureInvalidArgument(const ExecutionFailureInvalidArgument& other) = default;
    ExecutionFailureInvalidArgument(ExecutionFailureInvalidArgument&& other) noexcept = default;

    ExecutionFailureInvalidArgument& operator=(const ExecutionFailureInvalidArgument& other) = default;
    ExecutionFailureInvalidArgument& operator=(ExecutionFailureInvalidArgument&& other) noexcept = default;

    virtual void        Render(std::ostream& os, const Renderer& renderer) const = 0;
    virtual std::string Label(const Labeler& labeler) const = 0;

    virtual bool        is_success() const override;
};

class ExecutionFailureSignaled : public ExecutionFailure {
public:
    virtual ~ExecutionFailureSignaled() = default;

    ExecutionFailureSignaled (
        const std::filesystem::path& program,
        const std::string&           input,
        const std::string&           output,
        const rusage&                usage
    );
    ExecutionFailureSignaled(const ExecutionFailureSignaled& other) = default;
    ExecutionFailureSignaled(ExecutionFailureSignaled&& other) noexcept = default;

    ExecutionFailureSignaled& operator=(const ExecutionFailureSignaled& other) = default;
    ExecutionFailureSignaled& operator=(ExecutionFailureSignaled&& other) noexcept = default;

    virtual void        Render(std::ostream& os, const Renderer& renderer) const override;
    virtual std::string Label(const Labeler& labeler) const override;

    virtual bool        is_success() const override;
};

class ExecutionFailureSegmentationFault : public ExecutionFailureSignaled {
public:
    virtual ~ExecutionFailureSegmentationFault() = default;

    ExecutionFailureSegmentationFault (
        const std::filesystem::path& program,
        const std::string&           input,
        const std::string&           output,
        const rusage&                usage
    );
    ExecutionFailureSegmentationFault(const ExecutionFailureSegmentationFault& other) = default;
    ExecutionFailureSegmentationFault(ExecutionFailureSegmentationFault&& other) noexcept = default;

    ExecutionFailureSegmentationFault& operator=(const ExecutionFailureSegmentationFault& other) = default;
    ExecutionFailureSegmentationFault& operator=(ExecutionFailureSegmentationFault&& other) noexcept = default;

    virtual void        Render(std::ostream& os, const Renderer& renderer) const = 0;
    virtual std::string Label(const Labeler& labeler) const = 0;

    virtual bool        is_success() const override;
};

class ExecutionFailureAbort : public ExecutionFailureSignaled {
public:
    virtual ~ExecutionFailureAbort() = default;

    ExecutionFailureAbort (
        const std::filesystem::path& program,
        const std::string&           input,
        const std::string&           output,
        const rusage&                usage
    );
    ExecutionFailureAbort(const ExecutionFailureAbort& other) = default;
    ExecutionFailureAbort(ExecutionFailureAbort&& other) noexcept = default;

    ExecutionFailureAbort& operator=(const ExecutionFailureAbort& other) = default;
    ExecutionFailureAbort& operator=(ExecutionFailureAbort&& other) noexcept = default;

    virtual void        Render(std::ostream& os, const Renderer& renderer) const = 0;
    virtual std::string Label(const Labeler& labeler) const = 0;

    virtual bool        is_success() const override;
};

class ExecutionFailureInterrupt : public ExecutionFailureSignaled {
public:
    virtual ~ExecutionFailureInterrupt() = default;

    ExecutionFailureInterrupt (
        const std::filesystem::path& program,
        const std::string&           input,
        const std::string&           output,
        const rusage&                usage
    );
    ExecutionFailureInterrupt(const ExecutionFailureInterrupt& other) = default;
    ExecutionFailureInterrupt(ExecutionFailureInterrupt&& other) noexcept = default;

    ExecutionFailureInterrupt& operator=(const ExecutionFailureInterrupt& other) = default;
    ExecutionFailureInterrupt& operator=(ExecutionFailureInterrupt&& other) noexcept = default;

    virtual void        Render(std::ostream& os, const Renderer& renderer) const = 0;
    virtual std::string Label(const Labeler& labeler) const = 0;

    virtual bool        is_success() const override;
};

class ExecutionFailureTermination : public ExecutionFailureSignaled {
public:
    virtual ~ExecutionFailureTermination() = default;

    ExecutionFailureTermination (
        const std::filesystem::path& program,
        const std::string&           input,
        const std::string&           output,
        const rusage&                usage
    );
    ExecutionFailureTermination(const ExecutionFailureTermination& other) = default;
    ExecutionFailureTermination(ExecutionFailureTermination&& other) noexcept = default;

    ExecutionFailureTermination& operator=(const ExecutionFailureTermination& other) = default;
    ExecutionFailureTermination& operator=(ExecutionFailureTermination&& other) noexcept = default;

    virtual void        Render(std::ostream& os, const Renderer& renderer) const = 0;
    virtual std::string Label(const Labeler& labeler) const = 0;

    virtual bool        is_success() const override;
};

class ExecutionFailureKill : public ExecutionFailureSignaled {
public:
    virtual ~ExecutionFailureKill() = default;

    ExecutionFailureKill (
        const std::filesystem::path& program,
        const std::string&           input,
        const std::string&           output,
        const rusage&                usage
    );
    ExecutionFailureKill(const ExecutionFailureKill& other) = default;
    ExecutionFailureKill(ExecutionFailureKill&& other) noexcept = default;

    ExecutionFailureKill& operator=(const ExecutionFailureKill& other) = default;
    ExecutionFailureKill& operator=(ExecutionFailureKill&& other) noexcept = default;

    virtual void        Render(std::ostream& os, const Renderer& renderer) const = 0;
    virtual std::string Label(const Labeler& labeler) const = 0;

    virtual bool        is_success() const override;
};

std::shared_ptr<ExecutionResult> CreateExecutionResult (
    int                          status,
    const std::filesystem::path& program,
    const std::string&           input,
    const std::string&           output,
    const rusage&                usage
);

}

#endif