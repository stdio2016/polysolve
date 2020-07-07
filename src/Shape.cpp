#include "Shape.hpp"
#include <algorithm>

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

void Shape::sortCoords() {
  std::sort(coords.begin(), coords.end());
}

std::vector<Coord> Shape::getValidTranslations(int maxTile) const {
  if (maxTile <= 1) return coords;
  std::vector<Coord> all = coords;
  for (Coord &c : all) {
    c.x = 0;
  }
  auto new_end = std::unique(all.begin(), all.end());
  all.resize(new_end - all.begin());
  return all;
}
