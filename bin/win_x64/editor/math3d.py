import math

FLOAT_PRECISION=1e-2

def f_equals(a, b):
  return math.isclose(a, b, abs_tol=FLOAT_PRECISION)

class vec2_t:
  def __init__(self, x, y):
    self.x = x
    self.y = y
  
  def add(a, b):
    return vec2_t(a.x + b.x, a.y + b.y)
  
  def mul(a, b):
    return vec2_t(a.x * b.x, a.y * b.y)
  
  def __repr__(self):
    return "vec2_t({x}, {y})".format(x = round(self.x, 4), y = round(self.y, 4))
  
  def __str__(self):
    return self.__repr__()

class vec3_t:
  def __init__(self, x, y, z):
    self.x = x
    self.y = y
    self.z = z
  
  def length(self):
    return math.sqrt(vec3_t.dot(self, self))
  
  def normalize(self):
    return self.divf(self.length())
  
  def add(a, b):
    return vec3_t(a.x + b.x, a.y + b.y, a.z + b.z)
  
  def sub(a, b):
    return vec3_t(a.x - b.x, a.y - b.y, a.z - b.z)
  
  def div(a, b):
    return vec3_t(a.x / b.x, a.y / b.y, a.z / b.z)
  
  def mulf(a, b):
    return vec3_t(a.x * b, a.y * b, a.z * b)
  
  def divf(a, b):
    return vec3_t(a.x / b, a.y / b, a.z / b)
  
  def dot(a, b):
    return a.x * b.x + a.y * b.y + a.z * b.z
  
  def equals(a, b):
    return f_equals(a.x, b.x) and f_equals(a.y, b.y) and f_equals(a.z, b.z)
  
  def __repr__(self):
    return "vec3_t({x}, {y}, {z})".format(x = round(self.x, 4), y = round(self.y, 4), z = round(self.z, 4))
  
  def __str__(self):
    return self.__repr__()
