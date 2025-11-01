#pragma once

#ifdef _DEBUG

#include <iostream>
#include <cassert>

#define ENGINE_ASSERT(x, msg) assert((x) && msg)

template<typename ...Args>
void LOG(Args && ...args) {
    std::cout << "\033[1;34m[LOG]   \033[0m";
    (std::cout << ... << std::forward<Args>(args)) << std::endl;
}template<typename ...Args>


void LOG_OK(Args && ...args) {
    std::cout << "\033[1;32m[OK]    \033[0m";
    (std::cout << ... << std::forward<Args>(args)) << std::endl;
}


template<typename ...Args>
void LOG_WARN(Args && ...args) {
    std::cout << "\033[1;33m[WARN]  \033[0m";

    (std::cout << ... << std::forward<Args>(args)) << std::endl;
}


template<typename ...Args>
void LOG_ERROR(Args && ...args) {
    std::cout << "\033[1;31m[ERROR] \033[0m";
    (std::cout << ... << std::forward<Args>(args)) << std::endl;
}

#else

#define ENGINE_ASSERT(x, msg) ((void)0)

#define LOG()       ((void)0)
#define LOG_OK()    ((void)0)
#define LOG_WARN()  ((void)0)
#define LOG_ERROR(g) ((void)0)

#endif
