/**
 * Define polyominoes to tile a board. A polyomino can have many shapes.
 */
#pragma once
#ifndef POLYOMINO_HPP
#define POLYOMINO_HPP
#include <vector>
#include <string>
#include "Shape.hpp"
#include "GridType.hpp"

class Polyomino {
public:
  enum MobilityType {
    STATIONARY = 0,
    TRANSLATE = 1,
    ROTATE = 2,
    MIRROR = 3
  };
  int id;
  MobilityType mobility;
  int maxAmount;
  int minAmount;
  std::vector<Shape> morphs;
  std::vector<std::string> names;

  std::vector<Shape> transforms;
  
  Polyomino(): id(0), mobility(ROTATE), minAmount(1), maxAmount(1) {}
  
  void generateTransforms(const GridType *const grid);
};

#endif
