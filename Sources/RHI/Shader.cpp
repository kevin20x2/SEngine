//
// Created by kevin on 2024/8/11.
//

#include "Shader.h"

#include "RHIUtils.h"

FVertexShaderProgram::FVertexShaderProgram(const char* FilePath)
{
    ShaderModule = FRHIUtils::LoadHlslShaderByFilePath(FilePath, VK_SHADER_STAGE_VERTEX_BIT);
}

FPixelShaderProgram::FPixelShaderProgram(const char* FilePath)
{
    ShaderModule = FRHIUtils::LoadHlslShaderByFilePath(FilePath,VK_SHADER_STAGE_FRAGMENT_BIT);
}
