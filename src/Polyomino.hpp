/**
 * Define polyominoes to tile a board. A polyomino can have many shapes.
 */
#pragma once
#ifndef POLYOMINO_HPP
#define POLYOMINO_HPP
#include <vector>
#include "Shape.hpp"

class Polyomino {
public:
  enum MobilityType {
    STATIONARY = 0,
    TRANSLATE = 1,
    ROTATE = 2,
    MIRROR = 3
  };
  MobilityType mobility;
  int maxAmount;
  int minAmount;
  std::vector<Shape> morphs;
};

#endif
