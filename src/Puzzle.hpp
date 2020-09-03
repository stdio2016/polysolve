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
#include "Timing.hpp"

class Puzzle {
public:
  Puzzle();
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

  // solve
  DlxColumn *minfit();
  void dlxSolve();
  int enterBranch(int row);
  // input is the return value of enterBranch
  void leaveBranch(int removedRowCount);
  int targetLevel;
  int numSolution;
  std::vector<std::vector<int>> solutions;
  int numRows;
  long long attempts;
  long long dlxCounter;
  bool saveSolution;

private:
  std::vector<DlxCell *> removedRows;
  std::vector<int> solutionStack;
  void dlxSolveRecursive(int lv);
};

#endif
