//
// Created by vinkzheng on 2025/8/28.
//

#ifndef PLATFORM_H
#define PLATFORM_H
#include "Core/BaseTypes.h"


class FPlatform
{
public:
    static bool ExecuteCommand(const FString &command, FString &Result);

};



#endif //PLATFORM_H
