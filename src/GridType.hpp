/**
 * Linear transformation for many kinds of grids.
 */
#pragma once
#ifndef GRID_TYPE_HPP
#define GRID_TYPE_HPP
#include <string>
#include <vector>
#include "Coord.hpp"

// base class of all grids
class GridType {
public:
  // factory of all GridTypes
  static GridType *fromName(std::string name);

  // compute i-th rotation of a coord
  // actually it can get reflection, if i >= rotateCount
  virtual Coord rotate(Coord coord, int i) const = 0;

  // some information about the grid
  std::string name;
  int dimension;
  int rotateCount;
  int canReflect;
  std::vector<int> orbit;

protected:
  // constructor for subclasses
  GridType(std::string name, int dimension, int rotateCount, bool canReflect,
      std::vector<int> orbit):
    name(name),
    dimension(dimension),
    rotateCount(rotateCount),
    canReflect(canReflect),
    orbit(orbit) {}
};

class GridSquare : public GridType {
public:
  GridSquare(): GridType("square", 2, 4, true, {0}) {}
  
  Coord rotate(Coord coord, int orient) const override;
};

class GridCube : public GridType {
public:
  GridCube(): GridType("cube", 3, 24, true, {0}) {}
  
  Coord rotate(Coord coord, int orient) const override;
};

class GridTriangle : public GridType {
public:
  GridTriangle(): GridType("triangle", 2, 6, true, {0, 0}) {}
  
  Coord rotate(Coord coord, int orient) const override;
};

class GridHexagon : public GridType {
public:
  GridHexagon(): GridType("hexagon", 2, 6, true, {0}) {}
  
  Coord rotate(Coord coord, int orient) const override;
};

class GridTan : public GridType {
public:
  GridTan(): GridType("tan", 2, 4, true, {0, 0, 0, 0}) {}
  
  Coord rotate(Coord coord, int orient) const override;
};

#endif
