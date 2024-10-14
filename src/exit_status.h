#ifndef EXIT_STATUS_H
#define EXIT_STATUS_H

namespace oj {

enum class ExitStatus : int {
    COMPILATION_TARGET_UP_TO_DATE = 100,
    COMPILATION_SOURCE_NOT_EXIST  = 101,
    COMPILATION_EXEC_FAILURE      = 102,
    COMPILATION_DUP_FAILURE       = 103,

    EXECUTION_PROGRAM_NOT_EXIST   = 110,
    EXECUTION_INPUT_NOT_EXIST     = 111,
    EXECUTION_EXEC_FAILURE        = 112,
    EXECUTION_DUP_FAILURE         = 113,
    EXECUTION_TIMEOUT             = 114
};

int CreateExitStatus(ExitStatus exit_status) {
    return static_cast<int>(exit_status) << 8;
};

}

#endif