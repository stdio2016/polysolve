#include "Polyomino.hpp"
#include <map>
#include <algorithm>

void Polyomino::generateTransforms(const GridType *const grid) {
  int rots = 1;
  std::map<Shape, int> generated;
  if (mobility == STATIONARY || mobility == TRANSLATE) {
    rots = 1;
  }
  else if (mobility == ROTATE) {
    rots = grid->rotateCount;
  }
  else if (mobility == MIRROR) {
    rots = grid->rotateCount * (grid->canReflect ? 2 : 1);
  }
  for (Shape sh : morphs) {
    for (int orient = 0; orient < rots; orient++) {
      Shape gen = sh;
      for (Coord &c : gen.coords) {
        c = grid->rotate(c, orient);
      }
      gen.normalize(grid->orbit.size());
      std::sort(gen.coords.begin(), gen.coords.end());
      if (generated.count(gen) == 0) {
        transforms.push_back(gen);
        generated[gen] = 1;
      }
    }
  }
}
