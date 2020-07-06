#include "Shape.hpp"

void Shape::normalize(int maxTile) {
  if (coords.empty()) return ;
  Coord mini = coords[0];
  for (Coord c : coords) {
    if (c.x < mini.x) mini.x = c.x;
    if (c.y < mini.y) mini.y = c.y;
    if (c.z < mini.z) mini.z = c.z;
    if (c.w < mini.w) mini.w = c.w;
  }
  if (maxTile > 1) mini.x = 0;
  for (Coord &c : coords) {
    c.x -= mini.x;
    c.y -= mini.y;
    c.z -= mini.z;
    c.w -= mini.w;
  }
}
