import sys
import json
from mapfile import *
from worldmap import *
from tiledict import *

worldmap = worldmap_from_json(tiledict_from_json(json.load(open("tiledict.json"))), json.load(open("map_alton.json")))

map_save(worldmap, "map.map")
