#include "renderer.h"

#define MAX_SPRITES					32
#define VERTEX_BUFFER_SIZE	10 * 1024 * 1024

#include "log.h"
#include <iostream>

void gl_api_init()
{
	glewExperimental = GL_TRUE;
	
	GLenum glew_status = glewInit();
	
	if (glew_status != GLEW_OK)
    LOG_ERROR("renderer_t::renderer_t") << "failed to initialize GLEW: " << glewGetErrorString(glew_status);
}

renderer_t::renderer_t(const game_t &game, float aspect_ratio) :
  m_vertex_buffer(VERTEX_BUFFER_SIZE),
  m_game(game)
{
	
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  
  glCullFace(GL_FRONT);
  glDepthFunc(GL_LESS);
  
  const float fov = 8.0f;
  
	m_map_projection_matrix.init_orthogonal_perspective(
    -fov,
    +fov,
    +fov * aspect_ratio,
    -fov * aspect_ratio,
    +fov * aspect_ratio,
    -fov * aspect_ratio);
	
  m_sprite_projection_matrix.init_orthogonal(
    -fov,
    +fov,
    +fov * aspect_ratio,
    -fov * aspect_ratio,
    +fov * aspect_ratio,
    -fov * aspect_ratio);
  
  load_texture("editor/tilemap.png").bind();
}

void renderer_t::new_map(mapfile_t &mapfile)
{
  std::vector<vertex_t> mesh_map = mapfile.load_vertices();
  m_mesh_map = m_vertex_buffer.push_mesh(mesh_map);
}

void renderer_t::render_player_view()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  setup_view_matrix();
  
  render_map();
  render_sprite();
}

void renderer_t::setup_view_matrix()
{
  vec3_t inverted_origin = vec3_t(m_game.get_camera().view_origin * -1, 0.0f);
  float inverted_angle = -m_game.get_camera().view_angle;
  
  mat4_t matrix_rotate = mat4_t().init_rotation_z(inverted_angle);
  mat4_t matrix_translate = mat4_t().init_translation(inverted_origin);
  
  m_view_matrix = matrix_translate * matrix_rotate;
}

void renderer_t::render_map()
{
  mat4_t mvp = m_view_matrix * m_map_projection_matrix;
  m_shader.uniform_mvp(mvp);
  
  draw_mesh(m_mesh_map);
}

void renderer_t::render_sprite()
{
  const float sprite_z_level = -0.6f;
  const vec2_t texel_size = vec2_t(TEXEL_WIDTH, TEXEL_HEIGHT);
  
  // NOTE: diagonally slanted mesh on z-axis for perspective
  static const vec3_t sprite_pos[] = {
    vec3_t(+0.5f, +1.0f, -1.0f), vec3_t(+0.5f, -0.0f, +0.0f), vec3_t(-0.5f, +1.0f, -1.0f),
    vec3_t(-0.5f, -0.0f, +0.0f), vec3_t(-0.5f, +1.0f, -1.0f), vec3_t(+0.5f, -0.0f, +0.0f)
  };
  
  static const vec2_t sprite_uv[] = {
    vec2_t(1.0f, 0.0f), vec2_t(1.0f, 1.0f), vec2_t(0.0f, 0.0f),
    vec2_t(0.0f, 1.0f), vec2_t(0.0f, 0.0f), vec2_t(1.0f, 1.0f)
  };
  
  static vertex_t sprite_vertices[6 * MAX_SPRITES];
  
  mat4_t mvp = m_view_matrix * m_sprite_projection_matrix;
  m_shader.uniform_mvp(mvp);
  
  int num_vertices = 6 * m_game.num_entities();
  for (int i = 0; i < m_game.num_entities(); i++) {
    const sprite_t &sprite = m_game.get_sprite(i);
    
    if (!sprite.is_active())
      continue;
    
    for (int j = 0; j < 6; j++) {
      vec3_t sprite_size_pos = sprite_pos[j] * vec3_t(sprite.size, sprite.size.y);
      vec3_t sprite_rot_pos = sprite_size_pos.rotate_z(m_game.get_camera().view_angle);
      
      vec2_t sprite_uv_scaled = sprite_uv[j] * sprite.size;
      vec2_t sprite_uv_offset = sprite_uv_scaled + vec2_t(sprite.frame, sprite.state) + sprite.offset;
      
      vec3_t vertex_pos = sprite_rot_pos + vec3_t(sprite.position, 0);
      vec2_t vertex_uv = sprite_uv_offset * texel_size;
      
      sprite_vertices[i * 6 + j].pos = vertex_pos;
      sprite_vertices[i * 6 + j].uv = vertex_uv;
    }
  }
  
  m_vertex_buffer.sub_mesh(m_sprite_mesh, sprite_vertices, num_vertices);
  
  draw_mesh_dynamic(m_sprite_mesh, num_vertices);
}

void renderer_t::draw_mesh(const mesh_t &mesh)
{
  glDrawArrays(GL_TRIANGLES, mesh.offset, mesh.size);
}

void renderer_t::draw_mesh_dynamic(const mesh_t &mesh, int num_vertices)
{
  glDrawArrays(GL_TRIANGLES, mesh.offset, num_vertices);
}
