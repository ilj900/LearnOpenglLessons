#ifndef MODEL_H
#define MODEL_H

#include <glad.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <mesh.h>

unsigned int TextureFromFile(std::string path, std::string directory, bool gamma = false);

class Model
{
public:
    std::vector<Texture> textures_loaded;
    std::vector<Mesh> meshes;
    std::string directory;
    bool gammaCorrection;

    Model(std::string path, bool gamma = false);
    void Draw(std::string shaderName);
private:
    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};

#endif
