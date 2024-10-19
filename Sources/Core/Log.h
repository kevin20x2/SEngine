//
// Created by vinkzheng on 2024/10/19.
//

#ifndef LOG_H
#define LOG_H
#include <spdlog/spdlog.h>


#define SLogD(...) spdlog::info(__VA_ARGS__)
#define SLogW(...) spdlog::warn(__VA_ARGS__)
#define SLogE(...) spdlog::error(__VA_ARGS__)

//#define TEXT(...) __VA_ARGS__

#endif //LOG_H
