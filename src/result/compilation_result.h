#ifndef COMPILATION_RESULT_H
#define COMPILATION_RESULT_H

#include <filesystem>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include <sys/resource.h>

#include "renderer.h"
#include "labeler.h"

#include "result.h"

namespace oj {

class CompilationResult : public Result {
public:
    virtual ~CompilationResult() = default;

    CompilationResult (
        const std::string&           message,
        const std::string&           command,
        const std::filesystem::path& source,
        const std::filesystem::path& target
    );
    CompilationResult(const CompilationResult& other) = default;
    CompilationResult(CompilationResult&& other) noexcept = default;

    CompilationResult& operator=(const CompilationResult& other) = default;
    CompilationResult& operator=(CompilationResult&& other) noexcept = default;

    std::string           message() const;
    std::string           command() const;
    std::filesystem::path source() const;
    std::filesystem::path target() const;

    virtual void        Render(std::ostream& os, const Renderer& renderer) const = 0;
    virtual std::string Label(const Labeler& labeler) const = 0;

    virtual bool        is_success() const = 0;

private:
    const std::string&           message_;
    const std::string&           command_;
    const std::filesystem::path& source_;
    const std::filesystem::path& target_;
};

class CompilationSuccess : public CompilationResult {
public:
    virtual ~CompilationSuccess() = default;

    CompilationSuccess (
        const std::string&           message,
        const std::string&           command,
        const std::filesystem::path& source,
        const std::filesystem::path& target
    );
    CompilationSuccess(const CompilationSuccess& other) = default;
    CompilationSuccess(CompilationSuccess&& other) noexcept = default;

    CompilationSuccess& operator=(const CompilationSuccess& other) = default;
    CompilationSuccess& operator=(CompilationSuccess&& other) noexcept = default;

    virtual void        Render(std::ostream& os, const Renderer& renderer) const override;
    virtual std::string Label(const Labeler& labeler) const override;

    virtual bool        is_success() const override;
};

class CompilationTargetUpToDate : public CompilationSuccess {
    virtual ~CompilationTargetUpToDate() = default;

    CompilationTargetUpToDate (
        const std::string&           message,
        const std::string&           command,
        const std::filesystem::path& source,
        const std::filesystem::path& target
    );
    CompilationTargetUpToDate(const CompilationTargetUpToDate& other) = default;
    CompilationTargetUpToDate(CompilationTargetUpToDate&& other) noexcept = default;

    CompilationTargetUpToDate& operator=(const CompilationTargetUpToDate& other) = default;
    CompilationTargetUpToDate& operator=(CompilationTargetUpToDate&& other) noexcept = default;

    virtual void        Render(std::ostream& os, const Renderer& renderer) const override;
    virtual std::string Label(const Labeler& labeler) const override;

    virtual bool        is_success() const override;
};

class CompilationFailure : public CompilationResult {
public:
    virtual ~CompilationFailure() = default;

    CompilationFailure (
        const std::string&           message,
        const std::string&           command,
        const std::filesystem::path& source,
        const std::filesystem::path& target
    );
    CompilationFailure(const CompilationFailure& other) = default;
    CompilationFailure(CompilationFailure&& other) noexcept = default;

    CompilationFailure& operator=(const CompilationFailure& other) = default;
    CompilationFailure& operator=(CompilationFailure&& other) noexcept = default;

    virtual void        Render(std::ostream& os, const Renderer& renderer) const override;
    virtual std::string Label(const Labeler& labeler) const override;

    virtual bool        is_success() const override;
};

class CompilationSourceNotExist : public CompilationFailure {
public:
    virtual ~CompilationSourceNotExist() = default;

    CompilationSourceNotExist (
        const std::string&           message,
        const std::string&           command,
        const std::filesystem::path& source,
        const std::filesystem::path& target
    );
    CompilationSourceNotExist(const CompilationSourceNotExist& other) = default;
    CompilationSourceNotExist(CompilationSourceNotExist&& other) noexcept = default;

    CompilationSourceNotExist& operator=(const CompilationSourceNotExist& other) = default;
    CompilationSourceNotExist& operator=(CompilationSourceNotExist&& other) noexcept = default;

    virtual void        Render(std::ostream& os, const Renderer& renderer) const override;
    virtual std::string Label(const Labeler& labeler) const override;

    virtual bool        is_success() const override;
};

std::shared_ptr<CompilationResult> CreateCompilationResult (
    int                          status, 
    const std::string&           message, 
    const std::string&           command, 
    const std::filesystem::path& source, 
    const std::filesystem::path& target
);

}

#endif