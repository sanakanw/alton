from math3d import *
from mapdefs import *

def worldmap_from_json(worldmap_json):
  worldmap_width = worldmap_json['width']
  worldmap_height = worldmap_json['height']

  worldmap_tiles = worldmap_json['layers'][0]['data']
  
  return worldmap_t(worldmap_tiles, worldmap_width, worldmap_height)

class worldmap_t:
  def __init__(self, tiles, width, height):
    self.tiles = tiles
    self.width = width
    self.height = height
  
  def get_tile(self, x, y):
    if x < 0 or y < 0 or x >= self.width or y >= self.height:
      return 0
    
    return self.tiles[x + y * self.width]

  def find_adjacent_blocks(self, xpos, ypos):
    check = [
      ( 0,  1),
      (-1,  0),
      (+1,  0),
      ( 0, -1),
    ]
    
    adjacent = []
    for i in range(0, len(check)):
      x, y = check[i]
      
      if self.get_tile(xpos + x, ypos + y) == 2:
        adjacent.append(i)
    
    return adjacent
  
  def build_vertices(self):
    vertices = []
    
    uv = [
      vec2_t(0, 0),
      vec2_t(1, 0)
    ]
    
    for y in range(self.width):
      for x in range(self.height):
        tile_no = self.get_tile(x, y) - 1
        
        pos = vec3_t(x, y, 0)
        if tile_no == 0:
          vertices.extend(mesh_tile(pos, uv[tile_no]))
        elif tile_no == 1:
          vertices.extend(mesh_block(pos, self.find_adjacent_blocks(x, y), uv[tile_no]))
        else:
          print("unknown tile: tile_t(" + str(tile_no) + ") at " + str(pos))
    
    return vertices

def square_to_triangle(vertices):
  if len(vertices) != 4:
    print("square_to_triangle():", "not a square")
    sys.exit(1)
  
  return [
    vertices[0], vertices[2], vertices[1],
    vertices[3], vertices[1], vertices[2]
  ]
  
def mesh_square(pos, tangent, bitangent, uv):
  square_template = [
    (0, 0),
    (1, 0),
    (0, 1),
    (1, 1)
  ]
  
  vertices = []
  
  for x, y in square_template:
    vertex_pos = pos.add(tangent.mulf(x)).add(bitangent.mulf(y))
    vertex_uv = uv.add(vec2_t(x, 1-y)).mul(vec2_t(TEXEL_WIDTH, TEXEL_HEIGHT))
    
    vertices.append(mesh_vertex_t(vertex_pos, vertex_uv))
  
  return square_to_triangle(vertices)

def mesh_tile(pos, uv):
  return mesh_square(pos, vec3_t(1, 0, 0), vec3_t(0, 1, 0), uv)

def mesh_block(pos, omitted_blocks, uv):
  offset_tangent_bitangent = [
    (vec3_t( 1.0, +1.0,  0.0), vec3_t(-1.0,  0.0,  0.0), vec3_t( 0.0,  0.0, -1.0)), # front ( 0,  0, +1)
    (vec3_t( 0.0, +1.0,  0.0), vec3_t( 0.0, -1.0,  0.0), vec3_t( 0.0,  0.0, -1.0)), # left  (-1,  0,  0)
    (vec3_t(+1.0,  0.0,  0.0), vec3_t( 0.0,  1.0,  0.0), vec3_t( 0.0,  0.0, -1.0)), # right (+1,  0,  0)
    (vec3_t( 0.0,  0.0,  0.0), vec3_t(+1.0,  0.0,  0.0), vec3_t( 0.0,  0.0, -1.0)), # back  ( 0,  0, -1)
    (vec3_t( 0.0,  0.0, -1.0), vec3_t(+1.0,  0.0,  0.0), vec3_t( 0.0, +1.0,  0.0))  # top   ( 0, +1,  0)
  ]
  
  vertices = []
  
  for i in range(0, len(offset_tangent_bitangent)):
    if i in omitted_blocks:
      continue
    
    offset, tangent, bitangent = offset_tangent_bitangent[i]
    vertices.extend(mesh_square(pos.add(offset), tangent, bitangent, uv))
  
  return vertices
