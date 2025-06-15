#pragma once

#include <iostream>
#include <cassert>

#ifdef _DEBUG

#define ENGINE_ASSERT(x, msg) assert((x) && msg)

// Use ANSI escape codes for colors
#define LOG_OK(msg)    std::cout << "\033[1;32m[OK]    \033[0m" << msg << std::endl
#define LOG_WARN(msg)  std::cout << "\033[1;33m[WARN]  \033[0m" << msg << std::endl
#define LOG_ERROR(msg) std::cerr << "\033[1;31m[ERROR] \033[0m" << msg << std::endl

#else

#define ENGINE_ASSERT(x, msg) ((void)0)

#define LOG_OK(msg)    ((void)0)
#define LOG_WARN(msg)  ((void)0)
#define LOG_ERROR(msg) ((void)0)

#endif
