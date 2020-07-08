#include <iostream>
#include <fstream>
#include <exception>
#include <omp.h>
#include "CmdArgs.hpp"
#include "PuzzleReader.hpp"
#include "Timing.hpp"

static int solveOneFile(const CmdArgs &args, std::string filename, std::istream &puzzlefile) {
  Timing tm1;
  std::cout.precision(3);
  if (filename.empty()) filename = "from stdin";
  else filename = "\"" + filename + "\"";
  Puzzle puzzle;
  try {
    PuzzleReader reader;
    puzzle = reader.read(puzzlefile);
  }
  catch (std::exception& x) {
    std::cerr << "Error: Cannot parse puzzle file " << filename << ". Reason:"
              << std::endl;
    std::cerr << x.what() << std::endl;
    return 1;
  }
  std::cout << "parse time=" << std::fixed << tm1.getRunTime() << "ms\n";
  int i = 0;
  for (Polyomino &po : puzzle.polyominoes) {
    i += 1;
    po.generateTransforms(puzzle.grid);
    if (args.info) { 
      std::cout << "polyomino #" << i << '\n';
      for (Shape sh : po.transforms) {
        for (Coord c : sh.coords) {
          std::cout << "("<<c.x<<","<<c.y<<","<<c.z<<")";
        }
        std::cout <<" morph="<<sh.morph<<" orient="<<sh.orient << '\n';
      }
    }
  }
  puzzle.board.sortCoords();
  puzzle.buildDlxRows();
  std::cout << "build time=" << tm1.getRunTime() << "ms\n";
  if (args.info) {
    std::cout << "DLX rows=" << puzzle.dlxRows.size() << '\n';
    for (DlxRow &row : puzzle.dlxRows) {
      std::cout << "polyomino #" << row.polyomino
        <<" morph="<<row.morph<<" orient="<<row.orientation<<" coord=("
        <<row.position.x<<","<<row.position.y<<","<<row.position.z<<")"<< '\n';
    }
  }
  return 0;
}

int main(int argc, char *argv[]) {
  CmdArgs args;
  if (!args.parseCmdLine(argc, argv)) return 1;
  if (args.help) {
    args.showHelp();
    return 0;
  }
  if (args.ver) {
    args.showVersion();
    return 0;
  }
  
  int lastError = 1;
  for (std::string filename : args.filenames) {
    lastError = 0;
    if (args.info) {
      std::cout << "FILE=" << filename << "\n";
    }
    std::ifstream puzzlefile(filename);
    if (!puzzlefile) {
      std::cerr << "Error: File \"" << filename << "\" not found."
                << std::endl;
      lastError = 1;
      continue;
    }
    Timing tm;
    lastError = solveOneFile(args, filename, puzzlefile);
    std::cout << "total solve time: " << tm.getRunTime() << "ms\n";
  }
  if (args.filenames.empty()) {
    if (args.info) {
      std::cout << "FILE=stdin\n";
    }
    Timing tm;
    lastError = solveOneFile(args, "", std::cin);
    std::cout << "solve time: " << tm.getRunTime() << "ms\n";
  }
  return lastError;
}
