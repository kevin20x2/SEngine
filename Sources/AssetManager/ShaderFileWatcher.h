//
// Created by vinkzheng on 2024/10/28.
//

#ifndef SHADERFILEWATCHER_H
#define SHADERFILEWATCHER_H
#include "DirWatcherBase.h"
#include "Delegates.h"


class FShaderFileWatcher  : public FDirWatcherBase
{

    public:
    FShaderFileWatcher(const FString & InPath) : FDirWatcherBase(InPath) {} ;

    virtual void OnFileModified(const FString &InPath) override;

    using FOnShaderFileChanged = MulticastDelegate<const FString & >;

    FOnShaderFileChanged OnShaderFileChanged;
};



#endif //SHADERFILEWATCHER_H
