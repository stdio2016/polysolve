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
  
  template<int includeSelf=0> void unlinkRow(long long &unlinkCount);
  template<int includeSelf=0> void relinkRow();
  
  void setDown(DlxCell *n);
  
  int cover(long long &unlinkCount);
  int uncover();
};

struct DlxRow {
  int polyomino;
  int morph;
  int orientation;
  int transform;
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

inline void DlxCell::setDown(DlxCell *n) {
  down = n;
  up = n->up;
  n->up->down = this;
  n->up = this;
}

// GCC produces worse code for inlined unlinkRow/relinkRow
template <int includeSelf>
#if defined(__GNUC__) && !defined(__llvm__) && !defined(__INTEL_COMPILER)
__attribute((noinline))
#else
inline
#endif
void DlxCell::unlinkRow(long long &unlinkCount) {
  //printf("unlink %d\n", n->row);
  DlxCell *n = includeSelf ? this : this->right;

  int cnt = 0;
  DlxCell *right = n->right, *up = n->up, *down = n->down;
  DlxColumn *col = n->column;
  cnt += 1;
  up->down = down;
  down->up = up;
  n = right;
  while (n != this) {
    right = n->right, up = n->up, down = n->down;
    DlxColumn *col2 = n->column;
    int size = col->size;
    cnt += 1;
    up->down = down;
    down->up = up;
    col->size = size - 1;
    n = right;
    col = col2;
  }
  col->size -= 1;
  unlinkCount += cnt;
}

template <int includeSelf>
#if defined(__GNUC__) && !defined(__llvm__) && !defined(__INTEL_COMPILER)
__attribute((noinline))
#else
inline
#endif
void DlxCell::relinkRow() {
  //printf("relink %d\n", n->row);
  DlxCell *n = includeSelf ? this : this->right;

  DlxCell *right = n->right, *up = n->up, *down = n->down;
  DlxColumn *col = n->column;
  up->down = n;
  down->up = n;
  n = right;
  while (n != this) {
    right = n->right, up = n->up, down = n->down;
    DlxColumn *col2 = n->column;
    int size = col->size;
    up->down = n;
    down->up = n;
    col->size = size + 1;
    n = right;
    col = col2;
  }
  col->size += 1;
}

inline int DlxCell::cover(long long &unlinkCount) {
  //printf("cover %d\n", n->row);
  DlxCell *n2, *n3;
  DlxColumn *c2;
  //this->unlinkRow<true>(unlinkCount);
  n2 = this;
  int nrow = 0;
  do {
    c2 = n2->column;
    c2->value += 1;
    if (c2->value == c2->minValue) {
      c2->left->right = c2->right;
      c2->right->left = c2->left;
      unlinkCount += 1;
    }
    if (c2->value == c2->maxValue) {
      n3 = c2->down;
      while (n3 != c2) {
        n3->unlinkRow(unlinkCount);
        nrow += 1;
        n3 = n3->down;
      }
    }
    n2 = n2->right;
  } while (false && n2 != this) ;
  return nrow;
}

inline int DlxCell::uncover() {
  //printf("uncover %d\n", n->row);
  DlxCell *n2, *n3;
  DlxColumn *c2;
  n2 = this;
  int nrow = 0;
  do {
    //n2 = n2->left;
    c2 = n2->column;
    if (c2->value == c2->maxValue) {
      n3 = c2->up;
      while (n3 != c2) {
        n3->relinkRow();
        nrow += 1;
        n3 = n3->up;
      }
    }
    if (c2->value == c2->minValue) {
      c2->left->right = c2;
      c2->right->left = c2;
    }
    c2->value -= 1;
  } while (false && n2 != this) ;
  return nrow;
}

#endif
