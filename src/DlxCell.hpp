/**
 * Data structure for Dancing Links algorithm.
 */
#pragma once
#ifndef DLX_CELL_HPP
#define DLX_CELL_HPP
#include "Coord.hpp"

// forward declaration
struct DlxColumn;
struct DlxRow;

struct DlxCell {
  DlxCell *up, *down;
  DlxCell *left, *right;
  DlxColumn *column;
  DlxRow *row;
  
  bool isColumn() const;
};

struct DlxRow {
  int polyomino;
  int morph;
  int orientation;
  Coord position;
};

struct DlxColumn : DlxCell {
  int size;
  int value;
  int minValue, maxValue;
  Coord coord;
  int polyomino;
  
  inline DlxColumn *getLeft() const { return (DlxColumn*) left; }
  inline DlxColumn *getRight() const { return (DlxColumn*) right; }
};

inline bool DlxCell::isColumn() const { return this == column; }

#endif
