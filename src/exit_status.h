#ifndef EXIT_STATUS_H
#define EXIT_STATUS_H

namespace oj {

enum class ExitStatus : int {
    SUCCESS                     = EXIT_SUCCESS,
    FAILURE                     = EXIT_FAILURE,

    OUT_OF_MEMORY               = 100,
    TIMEOUT                     = 101,

    EXCEPTION                   = 110,
    EXCEPTION_BAD_ALLOC         = 111,
    EXCEPTION_OUT_OF_RANGE      = 112,
    EXCEPTION_LENGTH_ERROR      = 113,
    EXCEPTION_INVALID_ARGUMENT  = 114
};

}

#endif