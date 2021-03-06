from math3d import *

TEX_WIDTH = 256.0
TEX_HEIGHT = 256.0

SPRITE_WIDTH = 8.0
SPRITE_HEIGHT = 8.0

TEXEL_WIDTH = SPRITE_WIDTH / TEX_WIDTH
TEXEL_HEIGHT = SPRITE_HEIGHT / TEX_HEIGHT

ENTITY_TREE = 192

class map_entity_t:
  def __init__(self, entity_id, pos):
    self.entity_id = entity_id
    self.pos = pos

class mesh_vertex_t:
  def __init__(self, pos, uv):
    self.pos = pos
    self.uv = uv
  
  def __repr__(self):
    return "mesh_vertex_t({pos}, {uv})".format(pos = self.pos, uv = self.uv)
  
  def __str__(self):
    return self.__repr__()
