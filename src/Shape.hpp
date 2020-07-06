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
  
  void normalize(int maxTile);
};

inline bool operator==(const Shape &a, const Shape &b) {
  return a.coords == b.coords;
}

inline bool operator<(const Shape &a, const Shape &b) {
  return a.coords < b.coords;
}

#endif
