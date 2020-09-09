#include "Shape.hpp"
#include "GridType.hpp"
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
  std::sort(all.begin(), all.end());
  auto new_end = std::unique(all.begin(), all.end());
  all.resize(new_end - all.begin());
  return all;
}

bool Shape::validateCoords(GridType *grid, Coord &wrong) const {
  for (auto x : coords) {
    if (!grid->validateCoord(x)) {
      wrong = x;
      return false;
    }
  }
  return true;
}

bool Shape::validateNoDup(Coord &wrong) const {
  for (int i = 1; i < coords.size(); i++) {
    if (coords[i] == coords[i-1]) {
      wrong = coords[i];
      return false;
    }
  }
  return true;
}
