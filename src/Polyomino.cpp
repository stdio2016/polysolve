#include "Polyomino.hpp"
#include <map>
#include <algorithm>

void Polyomino::generateTransforms(const GridType *const grid) {
  int rots = 1;
  transforms.clear();
  std::map<Shape, int> generated;
  if (mobility == STATIONARY || mobility == TRANSLATE) {
    rots = 1;
  }
  else if (mobility == ROTATE) {
    rots = grid->rotateCount();
  }
  else if (mobility == MIRROR) {
    rots = grid->rotateCount() * (grid->canReflect() ? 2 : 1);
  }
  for (int i = 0; i < morphs.size(); i++) {
    Shape sh = morphs[i];
    sh.morph = i;
    for (int orient = 0; orient < rots; orient++) {
      Shape gen = sh;
      gen.orient = orient;
      for (Coord &c : gen.coords) {
        c = grid->rotate(c, orient);
      }
      // stationary piece uses absolute coordinates
      if (mobility != STATIONARY) {
        gen.normalize(grid->orbit().size());
      }
      gen.sortCoords();
      if (generated.count(gen) == 0) {
        transforms.push_back(gen);
        generated[gen] = 1;
      }
    }
  }
}
