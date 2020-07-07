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
