#include <map>
#include "GridType.hpp"

static std::map<std::string, GridType*> gridTypes = {
  {"square", new GridSquare()},
  {"cube", new GridCube()},
  {"triangle", new GridTriangle()},
  {"hexagon", new GridHexagon()},
  {"tan", new GridTan()},
  {"octagon", new GridOctagon()}
};

GridType *GridType::fromName(std::string name) {
  if (gridTypes.count(name)) {
    return gridTypes[name];
  }
  return nullptr;
}

bool GridType::validateCoord(Coord c) const {
  int ntiles = orbit().size();
  int dim = dimension();
  if (ntiles == 1) {
    // format: [x, y, z, w]
    if (dim <= 1 && c.y != 0) return false;
    if (dim <= 2 && c.z != 0) return false;
    if (dim <= 3 && c.w != 0) return false;
    return true;
  }
  // format: [tile, x, y, z]
  if (c.x >= ntiles || c.x < 0) return false;
  if (dim <= 1 && c.z != 0) return false;
  if (dim <= 2 && c.w != 0) return false;
  return true;
}

// square
Coord GridSquare::rotate(Coord c, int orient) const {
  if (orient & 4) {
    // reflection
    c = Coord{-c.x, c.y};
  }
  for (int i = 0; i < (orient&3); i++) {
    c = Coord{c.y, -c.x};
  }
  return c;
}

// cube
Coord GridCube::rotate(Coord c, int orient) const {
  int rr = (orient>>3) % 6;
  if (rr >= 3) {
    // reflection
    rr -= 3;
    c = Coord{-c.x, -c.y, -c.z};
  }
  switch (rr) {
    case 1: c = Coord{c.z, c.x, c.y}; break;
    case 2: c = Coord{c.y, c.z, c.x}; break;
  }
  if (orient & 4) {
    c = Coord{c.x, -c.y, -c.z};
  }
  for (int i = 0; i < (orient&3); i++) {
    c = Coord{c.y, -c.x, c.z};
  }
  return c;
}

// triangle
Coord GridTriangle::rotate(Coord c, int orient) const {
  orient %= 12;
  if (orient >= 6) {
    // reflection
    orient -= 6;
    c = Coord{c.x, c.z-c.y-c.x, c.z};
  }
  for (int i = 0; i < orient; i++) {
    c = Coord{1-c.x, c.y-c.z+c.x-1, c.y};
  }
  return c;
}

// hexagon
Coord GridHexagon::rotate(Coord c, int orient) const {
  orient %= 12;
  if (orient >= 6) {
    // reflection
    orient -= 6;
    c = Coord{c.y, c.x};
  }
  for (int i = 0; i < orient; i++) {
    c = Coord{c.x-c.y, c.x};
  }
  return c;
}

// tan
Coord GridTan::rotate(Coord c, int orient) const {
  if (orient & 4) {
    // reflection
    c = Coord{2-c.x, -c.y, c.z};
  }
  for (int i = 0; i < (orient&3); i++) {
    c = Coord{c.x+1, c.z, -c.y};
  }
  c.x &= 3;
  return c;
}

// octagon
Coord GridOctagon::rotate(Coord c, int orient) const {
  if (orient & 4) {
    // reflection
    c = Coord{c.x, -c.y-c.x, c.z};
  }
  for (int i = 0; i < (orient&3); i++) {
    c = Coord{c.x, c.z, -c.y-c.x};
  }
  return c;
}
