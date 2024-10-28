//
// Created by vinkzheng on 2024/10/28.
//

#ifndef SHADERFILEWATCHER_H
#define SHADERFILEWATCHER_H
#include "DirWatcherBase.h"


class FShaderFileWatcher  : public FDirWatcherBase
{

    public:
    FShaderFileWatcher(const FString & InPath) : FDirWatcherBase(InPath) {} ;

    virtual void OnFileModified(const FString &InPath) override;
};



#endif //SHADERFILEWATCHER_H
