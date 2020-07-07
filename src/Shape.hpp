/**
 * A polyomino can have many shapes.
 */
#pragma once
#ifndef SHAPE_HPP
#define SHAPE_HPP
#include <vector>
#include "Coord.hpp"

class Shape {
public:
  std::vector<Coord> coords;
  int morph;
  int orient;
  
  explicit Shape(int morph=0, int orient=0): morph(morph), orient(orient) {}
  
  // normalize coords so the minimum of each dimension is 0
  void normalize(int maxTile);
  
  // sort coords
  void sortCoords();
  
  // get translations for polyomino
  // assume coords are sorted
  std::vector<Coord> getValidTranslations(int maxTile) const;
};

inline bool operator==(const Shape &a, const Shape &b) {
  return a.coords == b.coords;
}

inline bool operator<(const Shape &a, const Shape &b) {
  return a.coords < b.coords;
}

#endif
