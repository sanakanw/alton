#include "renderer.h"

#include <iostream>
#include <SDL2/SDL_image.h>

texture_t load_texture(const char *path)
{
  SDL_Surface *bitmap = IMG_Load(path);
  
  if (!bitmap)
    std::cerr << "could not load " << path << std::endl;
  
  return texture_t(bitmap->pixels, bitmap->w, bitmap->h);
}

texture_t::texture_t(void *data, int width, int height)
{
  glGenTextures(1, &m_tex);
  glBindTexture(GL_TEXTURE_2D, m_tex);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
}

void texture_t::bind()
{
  glBindTexture(GL_TEXTURE_2D, m_tex);
}