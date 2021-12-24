from mapdefs import *
import struct

MAP_VERSION = 1
LUMP_TILES = 0
LUMP_VERTICES = 1
NUM_LUMPS = 2

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

def emit_mesh_vertex_t(f, vertex):
  emit_vec3_t(f, vertex.pos)
  emit_vec2_t(f, vertex.uv)

def emit_lump(f, fileofs, filelen):
  emit_i32(f, fileofs)
  emit_i32(f, filelen)

def map_save(map_width, map_height, tiles, mesh_vertices, path):
  with open(path, "wb") as f:
    f.seek(SIZEOF_HEADER)
    
    tiles_fileofs = f.tell()
    for tile in tiles:
      emit_i32(f, tile)
    tiles_filelen = f.tell() - tiles_fileofs
    
    vertices_fileofs = f.tell()
    for mesh_vertex in mesh_vertices:
      emit_mesh_vertex_t(f, mesh_vertex)
    vertices_filelen = f.tell() - vertices_fileofs
    
    f.seek(0)
    
    emit_i32(f, MAP_VERSION)
    emit_i32(f, map_width)
    emit_i32(f, map_height)
    emit_lump(f, tiles_fileofs, tiles_filelen)
    emit_lump(f, vertices_fileofs, vertices_filelen)
