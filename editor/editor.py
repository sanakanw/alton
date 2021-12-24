import sys
import json
from mapfile import *
from worldmap import *

worldmap = worldmap_from_json(json.load(open("map_alton.json")))

map_save(worldmap.width, worldmap.height, worldmap.tiles, worldmap.build_vertices(), "map.map")
