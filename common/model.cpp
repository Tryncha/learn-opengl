#include "model.h"

#include <assimp/material.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glad/glad.h>
#include <stb_image/stb_image.h>

#include <assimp/Importer.hpp>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

#include "mesh.h"
#include "shader.h"

// constructor, expects a filepath to a 3D model.
Model::Model(const std::string& path, bool gamma) : m_gamma(gamma) {
  loadModel(path);
}

// draws the model, and thus all its meshes
void Model::draw(const Shader& shader) {
  for (std::size_t i{0}; i < m_meshes.size(); ++i) {
    m_meshes[i].draw(shader);
  }
}

// loads a model with supported ASSIMP extensions from file
// and stores the resulting meshes in the meshes vector
void Model::loadModel(const std::string& path) {
  Assimp::Importer import{};
  const aiScene* scene{import.ReadFile(
      path, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                aiProcess_FlipUVs | aiProcess_CalcTangentSpace)};

  // check for errors
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    std::cout << "ERROR::ASSIMP:" << import.GetErrorString() << '\n';
    return;
  }

  // retrieve the directory path of the filepath
  m_directory = path.substr(0, path.find_last_of('/'));
  // process ASSIMP's root node recursively
  processNode(scene->mRootNode, scene);
}

// Processes a node in a recursive fashion.
// Processes each individual mesh located at the node and repeats
// this process on its children nodes (if any)
void Model::processNode(aiNode* node, const aiScene* scene) {
  // process all the node's meshes (if any)
  for (std::size_t i{0}; i < node->mNumMeshes; ++i) {
    aiMesh* mesh{scene->mMeshes[node->mMeshes[i]]};
    m_meshes.push_back(processMesh(mesh, scene));
  }

  // then do the same for each of its children
  for (std::size_t i{0}; i < node->mNumChildren; ++i) {
    processNode(node->mChildren[i], scene);
  }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
  std::vector<Vertex> vertices{};
  std::vector<unsigned int> indices{};
  std::vector<Texture> textures{};

  // mesh's vertices
  for (std::size_t i{0}; i < mesh->mNumVertices; ++i) {
    Vertex vertex{};

    // process vertex positions
    // we declare a placeholder vector since assimp uses its own vector class
    // that doesn't directly convert to glm's vec3 class
    glm::vec3 vertexPosition{};
    vertexPosition.x = mesh->mVertices[i].x;
    vertexPosition.y = mesh->mVertices[i].y;
    vertexPosition.z = mesh->mVertices[i].z;
    vertex.position = vertexPosition;

    // process vertex normals
    if (mesh->HasNormals()) {
      glm::vec3 vertexNormal{};
      vertexNormal.x = mesh->mNormals[i].x;
      vertexNormal.y = mesh->mNormals[i].y;
      vertexNormal.z = mesh->mNormals[i].z;
      vertex.normal = vertexNormal;
    }

    // process vertex texture coordinates
    if (mesh->mTextureCoords[0]) {
      glm::vec2 vertexTexCoords{};
      vertexTexCoords.x = mesh->mTextureCoords[0][i].x;
      vertexTexCoords.y = mesh->mTextureCoords[0][i].y;
      vertex.texCoords = vertexTexCoords;

      glm::vec3 vertexTangent{};
      vertexTangent.x = mesh->mTangents[i].x;
      vertexTangent.y = mesh->mTangents[i].y;
      vertexTangent.z = mesh->mTangents[i].z;
      vertex.tangent = vertexTangent;

      glm::vec3 vertexBitangent{};
      vertexBitangent.x = mesh->mBitangents[i].x;
      vertexBitangent.y = mesh->mBitangents[i].y;
      vertexBitangent.z = mesh->mBitangents[i].z;
      vertex.bitangent = vertexBitangent;
    } else {
      vertex.texCoords = glm::vec2(0.0f, 0.0f);
    }

    vertices.push_back(vertex);
  }

  // mesh's indices
  for (std::size_t i{0}; i < mesh->mNumFaces; ++i) {
    aiFace face{mesh->mFaces[i]};

    for (std::size_t j{0}; j < face.mNumIndices; ++j) {
      indices.push_back(face.mIndices[j]);
    }
  }

  // mesh's materials
  aiMaterial* material{scene->mMaterials[mesh->mMaterialIndex]};

  // 1. diffuse maps
  std::vector<Texture> diffuseMaps{
      loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse")};
  textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

  // 2. specular maps
  std::vector<Texture> specularMaps{loadMaterialTextures(
      material, aiTextureType_SPECULAR, "texture_specular")};
  textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

  // 3. normal maps
  std::vector<Texture> normalMaps{
      loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal")};
  textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

  // 4. height maps
  std::vector<Texture> heightMaps{
      loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height")};
  textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

  return Mesh(vertices, indices, textures);
}

unsigned int loadTextureFromFile(const char* texturePath,
                                 const std::string& directory) {
  std::string filename{std::string(texturePath)};
  filename = directory + '/' + filename;

  unsigned int textureId{};
  glGenTextures(1, &textureId);

  int width{};
  int height{};
  int nrChannels{};

  unsigned char* textureData{
      stbi_load(filename.c_str(), &width, &height, &nrChannels, 0)};

  if (textureData) {
    unsigned int format{};
    switch (nrChannels) {
      case 1:
        format = GL_RED;
        break;
      case 3:
        format = GL_RGB;
        break;
      case 4:
        format = GL_RGBA;
        break;
    }

    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<int>(format), width, height, 0,
                 format, GL_UNSIGNED_BYTE, textureData);

    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  } else {
    std::cerr << "Failed to load texture.\nCheck path: " << filename << '\n';
  }

  stbi_image_free(textureData);
  return textureId;
}

// Checks all material textures of a given type and loads the textures
// if they're not loaded yet.
// The required info is returned as a Texture struct.
std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat,
                                                 aiTextureType type,
                                                 const std::string& typeName) {
  std::vector<Texture> textures{};

  for (unsigned int i{0}; i < mat->GetTextureCount(type); ++i) {
    aiString str{};
    mat->GetTexture(type, i, &str);

    // check if texture was loaded before and if so, continue to next iteration:
    // skip loading a new texture
    bool skip{false};
    for (std::size_t j{0}; j < m_texturesLoaded.size(); ++j) {
      if (!std::strcmp(m_texturesLoaded[j].path.data(), str.C_Str())) {
        textures.push_back(m_texturesLoaded[j]);
        skip = true;
        break;
      }
    }

    // if texture hasn't been loaded already, load it
    if (!skip) {
      Texture texture{};
      texture.path = str.C_Str();
      texture.type = typeName;
      texture.id = loadTextureFromFile(str.C_Str(), m_directory);

      textures.push_back(texture);
      // store it as texture loaded for entire model, to ensure we won't
      // unnecessary load duplicate textures.
      m_texturesLoaded.push_back(texture);
    }
  }

  return textures;
}
