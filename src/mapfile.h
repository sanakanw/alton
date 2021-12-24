#ifndef FILE_MAP_H
#define FILE_MAP_H

#include "math3d.h"
#include "mapdefs.h"
#include <fstream>
#include <vector>

typedef enum {
  LUMP_TILES,
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

public:
  mapfile_t(std::ifstream &in);
  
  int get_width() const;
  int get_height() const;
  
  std::vector<tile_t> load_tiles();
  std::vector<vertex_t> load_vertices();
};

#endif
