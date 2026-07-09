#ifndef MODEL_H
#define MODEL_H

#include <assimp/material.h>
#include <assimp/scene.h>

#include <string>
#include <vector>

#include "mesh.h"
#include "shader.h"

class Model {
 public:
  // constructor, expects a filepath to a 3D model.
  Model(const std::string& path, bool gammaCorrection = false);

  // draws the model, and thus all its meshes
  void draw(const Shader& shader);

 private:
  // loads a model with supported ASSIMP extensions from file
  // and stores the resulting meshes in the meshes vector
  void loadModel(const std::string& path);

  // Processes a node in a recursive fashion.
  // Processes each individual mesh located at the node and repeats
  // this process on its children nodes (if any)
  void processNode(aiNode* node, const aiScene* scene);
  Mesh processMesh(aiMesh* mesh, const aiScene* scene);

  // Checks all material textures of a given type and loads the textures
  // if they're not loaded yet.
  // The required info is returned as a Texture struct.
  std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
                                            const std::string& typeName);

  std::vector<Mesh> m_meshes{};
  // stores all the textures loaded so far,
  // optimization to make sure textures aren't loaded more than once
  std::vector<Texture> m_texturesLoaded{};
  std::string m_directory{};
  bool m_gamma{};
};

#endif
