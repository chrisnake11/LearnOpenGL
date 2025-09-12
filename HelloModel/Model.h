//
// Created by zyx on 2025/9/11.
//
#ifndef MAIN_MODEL_H
#define MAIN_MODEL_H
#include <string>
#include <unordered_map>
#include <vector>
#include "Mesh.h"
#include "Shader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

class Model {
public:
    explicit Model(const std::string &path, bool gamma = false);
    void draw(const Shader& shader) const;
private:
    std::vector<Mesh> m_meshes;
    std::string m_directory;
    std::unordered_map<std::string, Texture> m_texture_loaded;
    bool gammaCorrection;
    void loadModel(const std::string &path);
    void processNode(const aiNode *node, const aiScene *scene);
    Mesh processMesh(const aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(const aiMaterial *mat, const aiTextureType type, const std::string &typeName);
};
#endif //MAIN_MODEL_H