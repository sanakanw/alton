#ifndef GAME_DEFS_H
#define GAME_DEFS_H

#include "math3d.h"

#define TEX_WIDTH     256.0
#define TEX_HEIGHT    256.0

#define SPRITE_WIDTH  8.0
#define SPRITE_HEIGHT 8.0

#define TILE_SNOW 1
#define TILE_WALL 2

#define ENTITY_TREE 192

#define TEXEL_WIDTH   (SPRITE_WIDTH / TEX_WIDTH)
#define TEXEL_HEIGHT  (SPRITE_HEIGHT / TEX_HEIGHT)

typedef int tile_t;

class map_entity_t {
public:
  int     entity_id;
  vec2_t  pos;
};

class vertex_t {
public:
  vec3_t  pos;
  vec2_t  uv;
};

#endif
