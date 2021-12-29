from math3d import *
from mapdefs import *

def get_uv_from_tile_id(tile_id, columns):
  return vec2_t(tile_id % columns, math.floor(tile_id / columns))

def worldmap_from_json(tiledict, worldmap_json):
  width = worldmap_json["width"]
  height = worldmap_json["height"]
  
  tiles = worldmap_json["layers"][0]["data"]
  entities = worldmap_json["layers"][1]["objects"]
  
  columns = worldmap_json["tilesets"][0]["columns"]
  
  return worldmap_t(width, height, columns, tiledict, tiles, entities)

class worldmap_t:
  def __init__(self, width, height, columns, tiledict, tiles, entities):
    self.width = width
    self.height = height
    self.columns = columns
    self.tiledict = tiledict
    self.tiles = tiles
    self.entities = entities
  
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
  
  def build_entities(self):
    entities = []
    
    for entity in self.entities:
      entity_id = int(entity["gid"])
      x_pos = entity["x"] / SPRITE_WIDTH
      y_pos = entity["y"] / SPRITE_HEIGHT
      
      entities.append(map_entity_t(entity_id, vec2_t(x_pos, y_pos)))
    
    return entities
  
  def build_vertices(self):
    vertices = []
    
    for y in range(self.width):
      for x in range(self.height):
        tile = self.get_tile(x, y) - 1
        
        pos = vec3_t(x, y, 0)
        tile_uv = get_uv_from_tile_id(tile, self.columns)
        
        if tile in self.tiledict:
          blocks = self.tiledict[tile]
          
          for block in blocks:
            block_uv = get_uv_from_tile_id(block, self.columns)
            vertices.extend(mesh_block(pos, self.find_adjacent_blocks(x, y), block_uv))
            pos.z -= 1
        
        vertices.extend(mesh_tile(pos, tile_uv))
    
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
    vertex_uv = uv.add(vec2_t(x * 0.99, y * 0.99)).mul(vec2_t(TEXEL_WIDTH, TEXEL_HEIGHT))
    
    #NOTE: multiplying by 0.99 for no weird pixel glitch resulting from floating precision
    
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
  ]
  
  vertices = []
  
  for i in range(0, len(offset_tangent_bitangent)):
    if i in omitted_blocks:
      continue
    
    offset, tangent, bitangent = offset_tangent_bitangent[i]
    vertices.extend(mesh_square(pos.add(offset), tangent, bitangent, uv))
  
  return vertices
