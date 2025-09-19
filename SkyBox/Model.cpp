//
// Created by zyx on 2025/9/11.
//
#include "Model.h"
#include <iostream>
#include <glm/glm.hpp>
#include <assimp/postprocess.h>
#include "stb_image.h"
#include <glad/glad.h>
#include <glfw/glfw3.h>

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

Model::Model(const std::string &path, bool gamma) : gammaCorrection(gamma){
    loadModel(path);
}

void Model::draw(const Shader &shader) const {
    for (const auto &mesh : m_meshes) {
        mesh.draw(shader);
    }
}

void Model::loadModel(const std::string &path) {
    // call import to read the model file
    Assimp::Importer importer;
    // flags: triangulate the model, flip the texture coordinates on y-axis
    const aiScene * scene = nullptr;
    scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    // check for errors
    // read file failed, incomplete scene or no root node
    if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }
    // get the directory path of the filepath
    m_directory = path.substr(0, path.find_last_of('/'));
    // process the root node recursively
    processNode(scene->mRootNode, scene);
}

void Model::processNode(const aiNode *node, const aiScene *scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        const aiMesh * mesh = scene->mMeshes[node->mMeshes[i]];
        m_meshes.push_back(processMesh(mesh, scene));
    }

    // recursively process each child node
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(const aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // load vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex{};
        glm::vec3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;

        if (mesh->HasNormals()) {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;
        }

        // load first set of texture coordinates only
        if (mesh->mTextureCoords[0]) {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoords = vec;
        }
        else {
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    // load indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // process materials
    // load material from scene
    const aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    // diffuse maps
    std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

    std::vector<Texture> reflectionMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_reflection");
    textures.insert(textures.end(), reflectionMaps.begin(), reflectionMaps.end());

    std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

    std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

    std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());


    return Mesh{vertices, indices, textures};
}

std::vector<Texture> Model::loadMaterialTextures(const aiMaterial *mat, const aiTextureType type,
    const std::string &typeName) {
    std::vector<Texture> textures;
    // load each texture from materials
    bool m_skit = false;

    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString path; // get texture path
        mat->GetTexture(type, i, &path);
        bool skip = false;
        // check texture is loaded before
        if (m_texture_loaded.count(path.C_Str())) {
            textures.push_back(m_texture_loaded[path.C_Str()]);
            skip = true;
        }
        if (!skip) {
            Texture texture{};
            texture.id = TextureFromFile(path.C_Str(), m_directory);
            texture.type = typeName;
            texture.path = path.C_Str();
            textures.push_back(texture);
            // store it as loaded
            m_texture_loaded[path.C_Str()] = texture;
        }
    }
    return textures;
}

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma) {
    std::string filename = path;
    filename = directory + "/" + filename;

    unsigned int texture;
    glGenTextures(1, &texture);

    int width{}, height{}, nrComponents{};
    // flip the texture on y-axis
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (!data) {
        std::cout << "Texture failed to load: " << filename << std::endl;
        std::cout << "ERROR: stbi_load failed: " << path << " reason=" << stbi_failure_reason() << std::endl;
        stbi_image_free(data);
        return texture;
    }

    GLenum format;
    if (nrComponents == 1) format = GL_RED;
    else if (nrComponents == 2) format = GL_RG;
    else if (nrComponents == 3) format = GL_RGB;
    else if (nrComponents == 4) format = GL_RGBA;
    else {
        std::cout << "Texture format not supported: " << filename << std::endl;
        stbi_image_free(data);
        return texture;
    }


    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return texture;
}