#include "renderer.h"

#include "log.h"

#define VERTEX_ATTRIB_0 3
#define VERTEX_ATTRIB_1 2

vertex_buffer_t::vertex_buffer_t(int size)
{
  glGenVertexArrays(1, &m_vao);
  glBindVertexArray(m_vao);
  
  glGenBuffers(1, &m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, size * sizeof(vertex_t), 0, GL_STATIC_DRAW);
  
  const int stride = sizeof(vertex_t);
  
  int offset = 0;
  
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, VERTEX_ATTRIB_0, GL_FLOAT, GL_FALSE, stride, (float*) 0 + offset);
  offset += VERTEX_ATTRIB_0;
  
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, VERTEX_ATTRIB_1, GL_FLOAT, GL_FALSE, stride, (float*) 0 + offset);
  offset += VERTEX_ATTRIB_1;
  
  m_vertex_ptr = size - 1;
}

const mesh_t vertex_buffer_t::allocate_mesh(int num_vertices)
{
  if (m_vertex_ptr - num_vertices < 0)
    LOG_ERROR("vertex_buffer_t::push_vertices") << "ran out of memory: " << m_vertex_ptr + num_vertices << "/" << num_vertices;
  
  m_vertex_ptr -= num_vertices;
  
  return mesh_t(m_vertex_ptr, num_vertices);
}

const mesh_t vertex_buffer_t::push_mesh(std::vector<vertex_t> &vertices)
{
  const mesh_t mesh = allocate_mesh(vertices.size());
  sub_mesh(mesh, vertices.data(), vertices.size());
  
  return mesh;
}

void vertex_buffer_t::sub_mesh(const mesh_t &mesh, vertex_t *vertices, int num_vertices)
{
  glBufferSubData(
    GL_ARRAY_BUFFER,
    mesh.offset * sizeof(vertex_t),
    num_vertices * sizeof(vertex_t),
    vertices);
}
