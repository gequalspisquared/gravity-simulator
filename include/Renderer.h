#ifndef RENDERER_H
#define RENDERER_H

#include "Shader.h"

#define SHADERS_LAST SHADER_BASIC_TEXTURE
enum ShaderType {
    SHADER_NONE,
    SHADER_SKYBOX,
    SHADER_BASIC_TEXTURE
};

class Renderer
{
public:
    Renderer() {}

    Shader shaders[SHADERS_LAST + 1];

private:
};

#endif