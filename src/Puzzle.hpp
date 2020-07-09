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
  
  // dancing links structure
  struct DLX {
    std::vector<DlxRow> rows;
    std::vector<DlxColumn> columns;
    std::vector<DlxCell> cells;
    
    // default constructor
    DLX() = default;
    
    // fix structure
    DLX(const DLX &other);
    DLX& operator=(const DLX &other);
    void swap(DLX &other) noexcept;
  } dlx;
  
  std::map<Coord, int> coordMap;
  
  // build Dancing Links data structure
  void buildDlxRows();
  void buildDlxColumns();
  void buildDlxCells();
};

#endif
