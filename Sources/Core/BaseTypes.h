//
// Created by kevin on 2024/8/11.
//

#ifndef BASETYPES_H
#define BASETYPES_H
#include <cstdint>
#include <memory>
#include <vector>

using int32 = int32_t;
using uint32 = uint32_t;
using uint16 = uint16_t;

template <typename T>
using TArray = std::vector<T>;

template <typename T>
using TSharedPtr = std::shared_ptr<T>;

template <typename T>
using TUniquePtr = std::unique_ptr<T>;

#endif //BASETYPES_H
