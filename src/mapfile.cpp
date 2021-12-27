#include "mapfile.h"

#include "log.h"
#include <iostream>

mapfile_t::mapfile_t(std::ifstream &in) :
  m_in(in)
{
  if (!in) {
    LOG_ERROR("mapfile_t::mapfile_t") << "failed to open file";
  }
  
  m_in.read((char*) &m_header, sizeof(mapheader_t));
}

int mapfile_t::get_width() const
{
  return m_header.map_width;
}

int mapfile_t::get_height() const
{
  return m_header.map_height;
}

void *mapfile_t::copy_lump(tlump_t tlump, int *lump_size)
{
  m_in.seekg(m_header.lumps[tlump].fileofs, m_in.beg);
  
  int filelen = m_header.lumps[tlump].filelen;
  
  void *lump_data = malloc(filelen);
  m_in.read((char*) lump_data, filelen);
  
  *lump_size = filelen;
  
  return lump_data;
}

std::vector<tile_t> mapfile_t::load_tiles()
{
  int tile_buffer_size;
  tile_t *tile_buffer = (tile_t*) copy_lump(LUMP_TILES, &tile_buffer_size);
  std::vector<tile_t> tiles(tile_buffer, tile_buffer + tile_buffer_size / sizeof(tile_t));
  free(tile_buffer);
  return tiles;
}

std::vector<vertex_t> mapfile_t::load_vertices()
{
  int vertex_buffer_size;
  vertex_t *vertex_buffer = (vertex_t*) copy_lump(LUMP_VERTICES, &vertex_buffer_size);
  std::vector<vertex_t> vertices(vertex_buffer, vertex_buffer + vertex_buffer_size / sizeof(vertex_t));
  free(vertex_buffer);
  return vertices;
}
