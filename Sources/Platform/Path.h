//
// Created by 郑文凯 on 2024/9/28.
//

#ifndef PATH_H
#define PATH_H


#include "Core/BaseTypes.h"
class FPath
{
public:
	static FString JoinPath(const FString & Dir , const FString & File );

	static FString GetApplicationDir() ;

	static FString GetDirectoryFromPath(const FString & Path);

	static FString GetEngineShaderDir();

	static FString GetAssetsDir();

};


#endif //PATH_H
