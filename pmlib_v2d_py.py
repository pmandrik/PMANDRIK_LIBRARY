#############################################################################
##  Author      :     P.S. Mandrik, IHEP
##  Date        :     08/03/20
##  Last Update :     08/03/20
##  Version     :     1.0
#############################################################################

# ======= v2 ====================================================================
from math import cos, sin, pi, acos, asin, hypot, sqrt, atan
from pmlib_const_py import *

class v2():
  def __init__(self, x=0, y=0, z=0):
    self.x, self.y, self.z = x, y, z

  def Rotate(self, angle):
    c, s = cos(angle), sin(angle)
    return v2( self.x * c + self.y * s,  - self.x * s +  self.y * c )

  def Rotated(self, angle):
    return self.Rotate(angle * PI_180)

  def Max(self):
    return max(self.x, self.y)

  def Min(self):
    return min(self.x, self.y)

  def L(self):
    return hypot(self.x, self.y)

  def L2(self):
    return self.x*self.x + self.y*self.y

  def AddList(self, other) : 
    self.x += other[0]
    self.y += other[1]
    return self

  def Tuple(self):
    return (self.x, self.y)

  def List(self):
    return [self.x, self.y]

  def Int(self):
    return v2(int(self.x), int(self.y), int(self.z))

  def __imul__(self, val):
    self.x *= val
    self.y *= val
    return self

  def __idiv__(self, val):
    self.x /= val
    self.y /= val
    return self

  def __iadd__(self, val):
    self.x += val.x
    self.y += val.y
    return self

  def __isub__ (self, val):
    self.x -= val.x
    self.y -= val.y
    return self

  def __mul__(self, val):
    return v2(self.x * val, self.y * val)

  def __div__(self, val):
    return v2(self.x / val, self.y / val)

  def __add__ (self, other):
    return v2(self.x + other.x, self.y + other.y)

  def __sub__ (self, other):
    return v2(self.x - other.x, self.y - other.y)

  def __eq__(self, other):
    if self.x == other.x and self.y == other.y : return True
    return False

  def __hash__(self):
    return self.x * 1000000000000 + self.y

  def __ne__(self, other):
    if self.x != other.x and self.y != other.y : return True
    return False

  def __neg__(self): 
    return v2(-self.x, -self.y)

  def __str__(self):
    return "v2(" + str(self.x) + ", " + str(self.y) + ")"

  def __len__(self):
    return 2

  def __getitem__(self, key):
    if key == 0 : return self.x
    if key == 1 : return self.y
    if key == 2 : return self.z
    return False

