/**
 * A puzzle, with solver
 */
#pragma once
#ifndef PUZZLE_HPP
#define PUZZLE_HPP
#include <vector>
#include "Coord.hpp"
#include "Polyomino.hpp"
#include "GridType.hpp"

class Puzzle {
public:
  std::vector<Coord> board;
  std::vector<Polyomino> polyominoes;
  GridType *grid;
};

#endif
