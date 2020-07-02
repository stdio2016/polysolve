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
};

#endif
