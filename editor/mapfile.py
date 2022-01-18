from mapdefs import *
import struct

MAP_VERSION = 1

LUMP_TILES = 0
LUMP_ENTITIES = 1
LUMP_VERTICES = 2

NUM_LUMPS = 3

SIZEOF_INT = 4
SIZEOF_LUMP = 2 * SIZEOF_INT
SIZEOF_HEADER = SIZEOF_INT + NUM_LUMPS * SIZEOF_LUMP

def emit_i32(f, i32):
  f.write(struct.pack('i', i32))

def emit_f32(f, f32):
  f.write(struct.pack('f', f32))

def emit_vec2_t(f, v):
  emit_f32(f, v.x)
  emit_f32(f, v.y)

def emit_vec3_t(f, v):
  emit_f32(f, v.x)
  emit_f32(f, v.y)
  emit_f32(f, v.z)

def emit_entity_t(f, entity):
  emit_i32(f, entity.entity_id)
  emit_vec2_t(f, entity.pos)

def emit_mesh_vertex_t(f, vertex):
  emit_vec3_t(f, vertex.pos)
  emit_vec2_t(f, vertex.uv)

def emit_lump(f, fileofs, filelen):
  emit_i32(f, fileofs)
  emit_i32(f, filelen)

def map_save(map, path):
  with open(path, "wb") as f:
    f.seek(SIZEOF_HEADER)
    
    tiles_fileofs = f.tell()
    for tile in map.tiles:
      emit_i32(f, tile)
    tiles_filelen = f.tell() - tiles_fileofs
    
    entities_fileofs = f.tell()
    for entity in map.build_entities():
      emit_entity_t(f, entity)
    entities_filelen = f.tell() - entities_fileofs
    
    vertices_fileofs = f.tell()
    for mesh_vertex in map.build_vertices():
      emit_mesh_vertex_t(f, mesh_vertex)
    vertices_filelen = f.tell() - vertices_fileofs
    
    f.seek(0)
    
    emit_i32(f, MAP_VERSION)
    emit_i32(f, map.width)
    emit_i32(f, map.height)
    emit_lump(f, tiles_fileofs, tiles_filelen)
    emit_lump(f, entities_fileofs, entities_filelen)
    emit_lump(f, vertices_fileofs, vertices_filelen)
