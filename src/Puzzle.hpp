/**
 * A puzzle, with solver
 */
#pragma once
#ifndef PUZZLE_HPP
#define PUZZLE_HPP
#include <vector>
#include <map>
#include "Shape.hpp"
#include "Polyomino.hpp"
#include "GridType.hpp"
#include "DlxCell.hpp"

class Puzzle {
public:
  Shape board;
  std::vector<Polyomino> polyominoes;
  GridType *grid;
  
  std::vector<DlxRow> dlxRows;
  std::vector<DlxColumn> dlxColumns;
  std::map<Coord, int> coordMap;
  
  // build Dancing Links data structure
  void buildDlxRows();
  void buildDlxColumns();
};

#endif
