//
// Created by vinkzheng on 2024/10/28.
//

#ifndef DIRWATCHERBASE_H
#define DIRWATCHERBASE_H
#include "Core/BaseTypes.h"

#include "FileWatch.hpp"

class FDirWatcherBase
{
public:
    FDirWatcherBase(const FString & InDirPath);

    virtual void OnFileModified(const FString & InPath) = 0 ;

protected:

    FString WatchDir;

    filewatch::FileWatch <FString > FileWatch;

};



#endif //DIRWATCHERBASE_H
