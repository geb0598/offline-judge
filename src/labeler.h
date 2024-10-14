#ifndef LABELER_H
#define LABELER_H

#include <ostream>

#include "result.h"
#include "compilation_result.h"
#include "execution_result.h"
#include "judge_result.h"
#include "submission_result.h"

namespace oj {

class Labeler {
    static Labeler& GetInstance() {
        static Labeler instance;
        return instance;
    }

    Labeler(const Labeler& other) = delete;
    Labeler(Labeler&& other) = delete;

    Labeler& operator=(const Labeler& other) = delete;
    Labeler& operator=(Labeler&& other) = delete;

    virtual void Label(std::ostream& os, const CompilationSuccess& result);
    virtual void Label(std::ostream& os, const CompilationFailure& result);
    
    virtual void Label(std::ostream& os, const ExecutionSuccess& result);
    virtual void Label(std::ostream& os, const ExecutionFailureTimeout& result);
    virtual void Label(std::ostream& os, const ExecutionFailureMemoryLimitExceeded& result);
    virtual void Label(std::ostream& os, const ExecutionFailureException& result);
    virtual void Label(std::ostream& os, const ExecutionFailureBadAlloc& result);
    virtual void Label(std::ostream& os, const ExecutionFailureOutofRange& result);
    virtual void Label(std::ostream& os, const ExecutionFailureLengthError& result);
    virtual void Label(std::ostream& os, const ExecutionFailureInvalidArgument& result);
    virtual void Label(std::ostream& os, const ExecutionFailureSignaled& result);
    virtual void Label(std::ostream& os, const ExecutionFailureSegmentationFault& result);
    virtual void Label(std::ostream& os, const ExecutionFailureAbort& result);
    virtual void Label(std::ostream& os, const ExecutionFailureInterrupt& result);
    virtual void Label(std::ostream& os, const ExecutionFailureTermination& result);
    virtual void Label(std::ostream& os, const ExecutionFailureKill& result);

    virtual void Label(std::ostream& os, const JudgeSuccess& result);
    virtual void Label(std::ostream& os, const JudgeFailureInvalidOutputFormat& result);
    virtual void Label(std::ostream& os, const JudgeFailureOutputExceeded& result);

    virtual void Label(std::ostream& os, const SubmissionSuccess& result);
    virtual void Label(std::ostream& os, const SubmissionFailure& result);

private:
    ~Labeler() = default;

    Labeler() = default;
};

class KoreanLabeler : public Labeler {

};

}

#endif