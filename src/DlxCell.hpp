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
  
  template<int includeSelf=0> void unlinkRow();
  template<int includeSelf=0> void relinkRow();
  
  void setDown(DlxCell *n);
  
  void cover();
  void uncover();
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

template <int includeSelf>
inline void DlxCell::unlinkRow() {
  //printf("unlink %d\n", n->row);
  DlxCell *n = includeSelf ? this : this->right;

  do {
    n->up->down = n->down;
    n->down->up = n->up;
    n->column->size -= 1;
    n = n->right;
  } while (n != this) ;
}

template <int includeSelf>
inline void DlxCell::relinkRow() {
  //printf("relink %d\n", n->row);
  DlxCell *n = includeSelf ? this : this->right;

  do {
    n->up->down = n;
    n->down->up = n;
    n->column->size += 1;
    n = n->right;
  } while (n != this) ;
}

inline void DlxCell::cover() {
  //printf("cover %d\n", n->row);
  DlxCell *n2, *n3;
  DlxColumn *c2;
  this->unlinkRow<true>();
  n2 = this;
  do {
    c2 = n2->column;
    c2->value += 1;
    if (c2->value == c2->minValue) {
      c2->left->right = c2->right;
      c2->right->left = c2->left;
    }
    if (c2->value == c2->maxValue) {
      n3 = c2->down;
      while (n3 != c2) {
        n3->unlinkRow();
        n3 = n3->down;
      }
    }
    n2 = n2->right;
  } while (n2 != this) ;
}

inline void DlxCell::uncover() {
  //printf("uncover %d\n", n->row);
  DlxCell *n2, *n3;
  DlxColumn *c2;
  n2 = this;
  do {
    n2 = n2->left;
    c2 = n2->column;
    if (c2->value == c2->maxValue) {
      n3 = c2->up;
      while (n3 != c2) {
        n3->relinkRow();
        n3 = n3->up;
      }
    }
    if (c2->value == c2->minValue) {
      c2->left->right = c2;
      c2->right->left = c2;
    }
    c2->value -= 1;
  } while (n2 != this) ;
}

#endif
