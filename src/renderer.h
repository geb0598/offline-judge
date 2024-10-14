#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <iostream>

#include "result.h"
#include "compilation_result.h"
#include "execution_result.h"
#include "judge_result.h"
#include "submission_result.h"

namespace oj {

class Renderer {
public:
    static Renderer& GetInstance() {
        static Renderer instance;
        return instance;
    }

    Renderer(const Renderer& other) = delete;
    Renderer(Renderer&& other) = delete;

    Renderer& operator=(const Renderer& other) = delete;
    Renderer& operator=(Renderer&& other) = delete;

    virtual void Render(std::ostream& os, const CompilationSuccess& result);
    virtual void Render(std::ostream& os, const CompilationFailure& result);
    
    virtual void Render(std::ostream& os, const ExecutionSuccess& result);
    virtual void Render(std::ostream& os, const ExecutionFailureTimeout& result);
    virtual void Render(std::ostream& os, const ExecutionFailureMemoryLimitExceeded& result);
    virtual void Render(std::ostream& os, const ExecutionFailureException& result);
    virtual void Render(std::ostream& os, const ExecutionFailureBadAlloc& result);
    virtual void Render(std::ostream& os, const ExecutionFailureOutofRange& result);
    virtual void Render(std::ostream& os, const ExecutionFailureLengthError& result);
    virtual void Render(std::ostream& os, const ExecutionFailureInvalidArgument& result);
    virtual void Render(std::ostream& os, const ExecutionFailureSignaled& result);
    virtual void Render(std::ostream& os, const ExecutionFailureSegmentationFault& result);
    virtual void Render(std::ostream& os, const ExecutionFailureAbort& result);
    virtual void Render(std::ostream& os, const ExecutionFailureInterrupt& result);
    virtual void Render(std::ostream& os, const ExecutionFailureTermination& result);
    virtual void Render(std::ostream& os, const ExecutionFailureKill& result);

    virtual void Render(std::ostream& os, const JudgeSuccess& result);
    virtual void Render(std::ostream& os, const JudgeFailureInvalidOutputFormat& result);
    virtual void Render(std::ostream& os, const JudgeFailureOutputExceeded& result);

    virtual void Render(std::ostream& os, const SubmissionSuccess& result);
    virtual void Render(std::ostream& os, const SubmissionFailure& result);

private:
    ~Renderer() = default;

    Renderer() = default;
};

class KoreanRenderer : public Renderer {

};

}

#endif