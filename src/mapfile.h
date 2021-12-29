#ifndef FILE_MAP_H
#define FILE_MAP_H

#include "math3d.h"
#include "mapdefs.h"
#include <fstream>
#include <vector>

typedef enum {
  LUMP_TILES,
  LUMP_ENTITIES,
  LUMP_VERTICES,
  MAX_LUMPS
} tlump_t;

class lump_t {
public:
  int fileofs;
  int filelen;
};

class mapheader_t {
public:
  int version;
  int map_width;
  int map_height;
  lump_t lumps[MAX_LUMPS];
};

class mapfile_t {
private:
  std::ifstream &m_in;
  mapheader_t   m_header;
  
  void *copy_lump(tlump_t tlump, int *lump_size);
  
  template<class T>
  std::vector<T> load_lump(tlump_t tlump)
  {
    int buffer_size;
    T *buffer = (T*) copy_lump(tlump, &buffer_size);
    std::vector<T> lump_vector(buffer, buffer + buffer_size / sizeof(T));
    free(buffer);
    return lump_vector;
  }

public:
  mapfile_t(std::ifstream &in);
  
  int get_width() const;
  int get_height() const;
  
  std::vector<tile_t> load_tiles();
  std::vector<vertex_t> load_vertices();
  std::vector<map_entity_t> load_entities();
};

#endif
