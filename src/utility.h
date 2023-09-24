#pragma once

#define ANSI_RESET "\033[0m"
#define ANSI_RED "\033[31m";
#define ANSI_GREEN "\033[32m"
#define ANSI_YELLOW "\033[33m"

#ifndef NDEBUG

#include <iostream>

#define DEBUG_MSG(msg) do { \
    std::cerr << ANSI_RED; \
    std::cerr << "[DEBUG MESSAGE]: " << __FILE__ << ":" << __LINE__ << " - " << msg << std::endl; \
    std::cerr << ANSI_RESET; \
} while (0)

#else

#define DEBUG_MSG(msg)

#endif