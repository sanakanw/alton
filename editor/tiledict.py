
def tiledict_from_json(json):
  tiledict = {} 
  for tiledata in json:
    tiledict[int(tiledata["id"])] = tiledata["blocks"]
  
  return tiledict
