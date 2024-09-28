//
// Created by kevin on 2024/8/11.
//

#ifndef BASETYPES_H
#define BASETYPES_H
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

using int32 = int32_t;
using uint32 = uint32_t;
using uint16 = uint16_t;

template <typename T>
using TArray = std::vector<T>;

template <typename T>
using TSharedPtr = std::shared_ptr<T>;

template <typename T>
using TUniquePtr = std::unique_ptr<T>;

template <typename T>
using TWeakPtr = std::weak_ptr<T>;

template <typename T>
using TFunction = std::function<T>;


template <typename Key, typename Value>
using TMap  = std::unordered_map<Key,Value>;

using FString = std::string;

#define S_DEFINE_ENUM_FLAGS(Enum) \
    inline bool EnumHasAnyFlags(Enum A, Enum B)\
    { return (((uint32)A) & ((uint32)B)) != 0 ; }\
    \
    inline void operator|=(Enum & Lhs ,Enum Rhs)\
    { Lhs = (Enum)(((uint32)Lhs) | ((uint32)Rhs)); }\
    inline Enum operator |(Enum Lhs ,Enum Rhs)\
    { return (Enum) ((((uint32)(Lhs)) | ((uint32)Rhs)));}





#endif //BASETYPES_H
