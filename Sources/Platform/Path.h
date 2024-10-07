//
// Created by 郑文凯 on 2024/9/28.
//

#ifndef PATH_H
#define PATH_H


#include "Core/BaseTypes.h"
class FPath
{
public:
	static FString GetApplicationDir() ;

	static FString GetDirectoryFromPath(const FString & Path);


};


#endif //PATH_H
