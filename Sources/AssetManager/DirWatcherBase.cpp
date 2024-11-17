//
// Created by vinkzheng on 2024/10/28.
//

#include "DirWatcherBase.h"

#include "Platform/Path.h"


FDirWatcherBase::FDirWatcherBase(const FString &InDirPath) :
WatchDir(InDirPath) , FileWatch(WatchDir,
    [this](const FString& Path ,const filewatch::Event ChangeType )
{
        if(ChangeType == filewatch::Event::modified)
        {
            this->OnFileModified(FPath::JoinPath(WatchDir,Path));
        }
} )
{
}
