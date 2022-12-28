#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "Shader.h"

typedef struct Vertex 
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
} Vertex;

typedef struct Texture 
{
    unsigned int id;
    std::string type;    
    std::string path;
} Texture;

class Mesh
{
public:
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture>      textures;

    Mesh(const std::vector<Vertex> &vertices, 
         const std::vector<unsigned int> &indices, 
         const std::vector<Texture> &textures);
    void draw(Shader &shader) const;

private:
    unsigned int VAO, VBO, EBO;

    void setupMesh();
};

#endif