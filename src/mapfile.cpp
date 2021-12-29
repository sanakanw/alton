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
  return load_lump<tile_t>(LUMP_TILES);
}

std::vector<map_entity_t> mapfile_t::load_entities()
{
  return load_lump<map_entity_t>(LUMP_ENTITIES);
}

std::vector<vertex_t> mapfile_t::load_vertices()
{
  return load_lump<vertex_t>(LUMP_VERTICES);
}
