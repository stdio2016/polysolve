#include "Puzzle.hpp"
#include <set>
#include <iostream>

void Puzzle::buildDlxRows() {
  dlxRows.clear();
  
  // convert boord coords to set
  std::set<Coord> boardCoords(board.coords.begin(), board.coords.end());
  
  // get valid movements
  int maxTile = grid->orbit().size();
  std::vector<Coord> validTrans = board.getValidTranslations(maxTile);
  
  // for each polyomino
  for (int i = 0; i < polyominoes.size(); i++) {
    const Polyomino &poly = polyominoes[i];
    int howMany = 0;
    // for each transformed morph
    for (const Shape &trans : poly.transforms) {
      // empty shape!
      if (trans.coords.empty()) continue ;
      
      // try each translation
      bool first = true;
      for (Coord coord : validTrans) {
        // HACK to process stationary pieces
        if (poly.mobility == Polyomino::STATIONARY) {
          if (!first) break;
          coord = Coord{0, 0, 0, 0};
        }
        first = false;
        bool inside = true;
        Coord c0 = trans.coords[0];
        if (maxTile > 1) c0.x = 0;
        
        // check if all translated coords are inside the board
        for (Coord c : trans.coords) {
          Coord cc = coord - c0 + c;
          auto match = boardCoords.find(coord - c0 + c);
          if (match == boardCoords.end()) {
            inside = false;
            break;
          }
        }
        if (inside) {
          DlxRow row;
          row.polyomino = i;
          row.morph = trans.morph;
          row.orientation = trans.orient;
          row.position = coord;
          dlxRows.push_back(row);
          howMany += 1;
        }
      } // try each translation
    } // for each transformed morph
  } // for each polyomino
}

void Puzzle::buildDlxColumns() {
  // root node
  dlxColumns.clear();
  dlxColumns.push_back(DlxColumn{});
  dlxColumns[0].polyomino = -1;
  dlxColumns[0].coord.x = -1;

  // column for each board tile
  int n = board.coords.size();
  for (int i = 0; i < n; i++) {
    Coord pos = board.coords[i];
    DlxColumn col{};
    col.minValue = 1;
    col.maxValue = 1;
    col.coord = pos;
    col.polyomino = -1;
    dlxColumns.push_back(col);
  }
  
  // column for each polyomino
  int m = polyominoes.size();
  for (int i = 0; i < m; i++) {
    const Polyomino &poly = polyominoes[i];
    DlxColumn col{};
    col.minValue = poly.minAmount;
    col.maxValue = poly.maxAmount;
    col.coord = Coord{-1};
    col.polyomino = i;
    dlxColumns.push_back(col);
  }
  
  // build linked list structure
  for (int i = 0; i < n+m+1; i++) {
    dlxColumns[i].up = dlxColumns[i].down = &dlxColumns[i];
    if (i == 0)
      dlxColumns[i].left = &dlxColumns[n+m];
    else
      dlxColumns[i].left = &dlxColumns[i-1];
    if (i == n+m)
      dlxColumns[i].right = &dlxColumns[0];
    else
      dlxColumns[i].right = &dlxColumns[i+1];
    dlxColumns[i].row = nullptr;
    dlxColumns[i].column = &dlxColumns[i];
    dlxColumns[i].size = 0;
    dlxColumns[i].value = 0;
  }
  
  // remove columns whose minValue are 0
  for (int i = 1; i < n+m+1; i++) {
    if (dlxColumns[i].minValue == 0) {
      DlxColumn *left = dlxColumns[i].getLeft();
      DlxColumn *right = dlxColumns[i].getRight();
      left->right = right;
      right->left = left;
    }
  }
}
