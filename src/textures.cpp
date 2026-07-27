#include "textures.h"

#include <glad/glad.h>
#include <stb_image.h>

#include <iostream>

unsigned int loadTexture(const char* texturePath, bool hasGammaCorrection) {
  int width{};
  int height{};
  int nrChannels{};

  unsigned int textureId{};
  glGenTextures(1, &textureId);
  stbi_set_flip_vertically_on_load(true);

  unsigned char* textureData{
      stbi_load(texturePath, &width, &height, &nrChannels, 0)};

  if (textureData) {
    unsigned int internalFormat{};
    unsigned int dataFormat{};
    switch (nrChannels) {
      case 1:
        internalFormat = GL_RED;
        dataFormat = GL_RED;
        break;

      case 3:
        internalFormat = hasGammaCorrection ? GL_SRGB : GL_RGB;
        dataFormat = GL_RGB;
        break;

      case 4:
        internalFormat = hasGammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
        dataFormat = GL_RGBA;
        break;
    }

    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<int>(internalFormat), width,
                 height, 0, dataFormat, GL_UNSIGNED_BYTE, textureData);

    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  } else {
    std::cerr << "Failed to load texture.\nCheck path: " << texturePath << '\n';
  }

  stbi_image_free(textureData);
  return textureId;
}
