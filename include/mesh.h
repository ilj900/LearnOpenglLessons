#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>

#include <string>
#include <vector>

#include <shader.h>

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    unsigned int VAO;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    void Draw(std::string programName);
	void DrawTextureless(std::string programName);
    void Draw(std::string programName, unsigned int nrOfInstances);

private:
    unsigned int VBO, EBO;

    void setupMesh();
};

#endif
