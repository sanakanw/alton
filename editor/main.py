import sys
import json
import sys
from map import *
from mapfile import *
from tiledict import *

if len(sys.argv) < 3:
  print("usage: {} [tiledict] [map] [output]".format(sys.argv[0]))
  sys.exit(0)

tiledict_json = json.load(open(sys.argv[1]))
map_json = json.load(open(sys.argv[2]))

map = map_from_json(tiledict_from_json(tiledict_json), map_json)

map_save(map, sys.argv[3])
