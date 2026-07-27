#include "textures.h"

#include <glad/glad.h>
#include <stb_image.h>

#include <iostream>

unsigned int loadTexture(const char* texturePath) {
  int width{};
  int height{};
  int nrChannels{};

  unsigned int textureId{};
  glGenTextures(1, &textureId);
  stbi_set_flip_vertically_on_load(true);

  unsigned char* textureData{
      stbi_load(texturePath, &width, &height, &nrChannels, 0)};

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

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                    format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  } else {
    std::cerr << "Failed to load texture.\nCheck path: " << texturePath << '\n';
  }

  stbi_image_free(textureData);
  return textureId;
}
