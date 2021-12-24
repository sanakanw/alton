#ifndef RENDERER_H
#define RENDERER_H

#include "game.h"
#include "mapdefs.h"
#include "mapfile.h"

#define GL_GLEXT_PROTOTYPES

#include <SDL2/SDL_opengl.h>

class mesh_t {
public:
  GLuint offset, size;

public:
  mesh_t()
  {
    
  }
  
  mesh_t(GLuint _offset, GLuint _size)
  {
    offset = _offset;
    size = _size;
  }
};

class vertex_buffer_t {
private:
  GLuint  m_vbo;
  GLuint  m_vao;
  GLuint  m_vertex_ptr;

public:
  vertex_buffer_t(int size);
  
  void sub_vertices(const mesh_t &mesh, std::vector<vertex_t> &vertices);
  const mesh_t allocate_mesh(int num_vertices);
  const mesh_t push_mesh(std::vector<vertex_t> &vertices);
  void sub_mesh(const mesh_t &mesh, vertex_t *vertices, int num_vertices);
};

class texture_t {
private:
  GLuint m_tex;

public:
  texture_t(void *data, int width, int height);
  void bind();
};

class shader_t {
private:
  GLuint  m_program;
  GLuint  m_uloc_mvp;

public:
  shader_t();
  
  void uniform_mvp(const mat4_t &mvp);
};

class renderer_t {
private:
  vertex_buffer_t m_vertex_buffer;
  shader_t        m_shader;
  mesh_t          m_mesh_map;
  mesh_t          m_sprite_mesh;
  
  mat4_t          m_map_projection_matrix;
  mat4_t          m_sprite_projection_matrix;
  
  const game_t    &m_game;
  
  void render_map();
  void render_sprite();
  void draw_mesh(const mesh_t &mesh);
  void draw_mesh_dynamic(const mesh_t &mesh, int num_vertices);
  
public:
  renderer_t(const game_t &game, float aspect_ratio);
  
  void new_map(mapfile_t &mapfile);
  
  void render_player_view();
};

texture_t load_texture(const char *path);
#endif
