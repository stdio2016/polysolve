#include "Puzzle.hpp"
#include <set>
#include <iostream>
#include <utility>

void Puzzle::buildDlxRows() {
  dlx.rows.clear();
  
  // convert boord coords to set
  std::set<Coord> boardCoords(board.coords.cbegin(), board.coords.cend());
  
  // get valid movements
  int maxTile = grid->orbit().size();
  std::vector<Coord> validTrans = board.getValidTranslations(maxTile);
  
  // for each polyomino
  for (int i = 0; i < polyominoes.size(); i++) {
    const Polyomino &poly = polyominoes[i];
    int howMany = 0;
    // for each transformed morph
    for (int j = 0; j < poly.transforms.size(); j++) {
      const Shape &trans = poly.transforms[j];
      // empty shape!
      if (trans.coords.empty()) continue ;
      
      // try each translation
      bool first = true;
      for (Coord coord : validTrans) {
        bool inside = true;
        Coord c0 = trans.coords[0];
        if (maxTile > 1) c0.x = 0;
        // HACK to process stationary pieces
        if (poly.mobility == Polyomino::STATIONARY) {
          if (!first) break;
          coord = c0;
        }
        first = false;
        
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
          row.transform = j;
          row.position = coord - c0;
          dlx.rows.push_back(row);
          howMany += 1;
        }
      } // try each translation
    } // for each transformed morph
  } // for each polyomino
}

void Puzzle::buildDlxColumns() {
  // root node
  dlx.columns.clear();
  dlx.columns.push_back(DlxColumn{});
  dlx.columns[0].polyomino = -1;
  dlx.columns[0].coord.x = -1;

  // column for each board tile
  int n = board.coords.size();
  for (int i = 0; i < n; i++) {
    Coord pos = board.coords[i];
    DlxColumn col{};
    col.minValue = 1;
    col.maxValue = 1;
    col.coord = pos;
    col.polyomino = -1;
    coordMap[pos] = i;
    dlx.columns.push_back(col);
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
    dlx.columns.push_back(col);
  }
  
  // build linked list structure
  for (int i = 0; i < n+m+1; i++) {
    dlx.columns[i].up = dlx.columns[i].down = &dlx.columns[i];
    if (i == 0)
      dlx.columns[i].left = &dlx.columns[n+m];
    else
      dlx.columns[i].left = &dlx.columns[i-1];
    if (i == n+m)
      dlx.columns[i].right = &dlx.columns[0];
    else
      dlx.columns[i].right = &dlx.columns[i+1];
    dlx.columns[i].row = nullptr;
    dlx.columns[i].column = &dlx.columns[i];
    dlx.columns[i].size = 0;
    dlx.columns[i].value = 0;
  }
  
  // remove columns whose minValue are 0
  for (int i = 1; i < n+m+1; i++) {
    if (dlx.columns[i].minValue == 0) {
      DlxColumn *left = dlx.columns[i].getLeft();
      DlxColumn *right = dlx.columns[i].getRight();
      left->right = right;
      right->left = left;
    }
  }
}

void Puzzle::buildDlxCells() {
  // count cells needed
  int n = board.coords.size();
  int need = 0;
  for (const DlxRow &row : dlx.rows) {
    need += polyominoes[row.polyomino].morphs[row.morph].coords.size() + 1;
  }
  dlx.cells.resize(need);
  need = 0;
  for (DlxRow &row : dlx.rows) {
    const Shape &sh = polyominoes[row.polyomino].transforms[row.transform];
    int size = sh.coords.size() + 1;
    for (const Coord &coord : sh.coords) {
      DlxCell &cell = dlx.cells[need];
      Coord newcoord = coord + row.position;
      DlxColumn &col = dlx.columns[1 + coordMap[newcoord]];
      cell.setDown(&col);
      cell.left = &cell - 1;
      cell.right = &cell + 1;
      cell.row = &row;
      cell.column = &col;
      col.size += 1;
      need += 1;
    }
    {
      DlxCell &cell = dlx.cells[need];
      DlxColumn &col = dlx.columns[1 + n + row.polyomino];
      cell.setDown(&col);
      cell.left = &cell - 1;
      cell.right = &cell + 1;
      cell.row = &row;
      cell.column = &col;
      col.size += 1;
      need += 1;
    }
    dlx.cells[need - size].left = &dlx.cells[need - 1];
    dlx.cells[need - 1].right = &dlx.cells[need - size];
  }
}

Puzzle::DLX::DLX(const DLX &other):
  // first copy nodes
  rows(other.rows), columns(other.columns), cells(other.cells) {
  // then remap up/down/left/right links
  DlxColumn *const mycol = columns.data();
  DlxCell *const mycell = cells.data();
  DlxRow *const myrow = rows.data();
  // const_cast just for pointer arithmetic
  DlxColumn *const ocol = const_cast<DlxColumn *>(other.columns.data());
  DlxCell *const ocell = const_cast<DlxCell *>(other.cells.data());
  DlxRow *const orow = const_cast<DlxRow *>(other.rows.data());
  
  int colcount = 0;
  const auto fixCellOrColumn = [&](DlxCell *&x) {
    // column of x is x if and only of x is column
    if (x->column == x) {
      colcount += 1;
      x = (DlxColumn*)x - ocol + mycol;
    }
    else {
      x = (DlxCell*)x - ocell + mycell;
    }
  };
  
  for (DlxColumn &col : columns) {
    fixCellOrColumn(col.up);
    fixCellOrColumn(col.down);
    col.left = col.getLeft() - ocol + mycol;
    col.right = col.getRight() - ocol + mycol;
    col.row = nullptr;
    col.column = &col;
  }
  for (DlxCell &cell : cells) {
    fixCellOrColumn(cell.up);
    fixCellOrColumn(cell.down);
    cell.left = cell.left - ocell + mycell;
    cell.right = cell.right - ocell + mycell;
    cell.row = cell.row - orow + myrow;
    cell.column = cell.column - ocol + mycol;
  }
}

// copy-and-swap idiom
Puzzle::DLX &Puzzle::DLX::operator=(const DLX &other) {
  if (this != &other) {
    DLX(other).swap(*this);
  }
  return *this;
}

void Puzzle::DLX::swap(DLX &other) noexcept {
  std::swap(rows, other.rows);
  std::swap(columns, other.columns);
  std::swap(cells, other.cells);
}

DlxColumn *Puzzle::minfit() {
  DlxColumn *root = &dlx.columns[0];
  DlxColumn *c = root->getRight();
  int minfit = c->size;
  for (DlxColumn *col = c->getRight();
      col != root; col = col->getRight()) {
    int value = col->value;
    // unsatisfiable
    if (value + col->size < col->minValue) {
      return nullptr;
    }
    // min fit
    if (col->size < minfit) {
      c = col;
      minfit = col->size;
    }
  }
  return c;
}

void Puzzle::dlxSolve() {
  // should be enough space
  removedRows.reserve(dlx.rows.size());
  dlxSolveRecursive();
  removedRows.clear();
}

void Puzzle::dlxSolveRecursive() {
  DlxColumn *root = &dlx.columns[0];
  if (root->right == root) {
    numSolution += 1;
    return ;
  }
  DlxColumn *col = minfit();
  if (!col) return; //unsatisfiable
  DlxCell *ptr = col->down;
  int removeCount = col->size;
  while (ptr != col) {
    DlxRow row = *ptr->row;
    ptr->cover();
    dlxSolveRecursive();
    ptr->uncover();
    removedRows.push_back(ptr);
    ptr = ptr->down;
  }
  for (int i = removeCount - 1; i >= 0; i--) {
    removedRows.back()->relinkRow<true>();
    removedRows.pop_back();
  }
}
