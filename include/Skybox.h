#ifndef SKYBOX_H
#define SKYBOX_H

#include "Cubemap.h"
#include "Shader.h"

class Skybox 
{
public:
    Skybox(const std::vector<std::string> &faces);
    void draw(Shader &shader) const;

private:
    unsigned int VAO, VBO, cubemapTexture;

    void setupSkybox();
};

#endif