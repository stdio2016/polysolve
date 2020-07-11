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
  std::cout << "parse time=" << std::fixed << tm1.getRunTime() << "ms" << std::endl;
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
  puzzle.buildDlxColumns();
  puzzle.buildDlxCells();
  std::cout << "build time=" << tm1.getRunTime() << "ms"<<std::endl;
  // create subproblem by expanding 3 levels of search tree
  puzzle.targetLevel = args.parallelLevel;
  puzzle.dlxSolve();
  std::vector<std::vector<int>> subproblems = puzzle.solutions;
  std::cout << "create subproblem time=" << tm1.getRunTime() << "ms"<<std::endl;
  int numSolution = 0;
  puzzle.targetLevel = -1;  // reset search tree depth
  puzzle.solutions.clear();
  #pragma omp parallel firstprivate(puzzle, tm1)
  {
    #pragma omp master
    std::cout << "copy time=" << tm1.getRunTime() << "ms"<<std::endl;
    
    std::vector<int> removedRowCount(args.parallelLevel);
    
    #pragma omp for schedule(static, 1) reduction(+:numSolution)
    for (int i = 0; i < subproblems.size(); i++) {
      if (subproblems[i].size() != args.parallelLevel) {
        numSolution += 1;
        continue;
      }
      
      // set up subproblem
      for (int j = 0; j < args.parallelLevel; j++)
        removedRowCount[j] = puzzle.enterBranch(subproblems[i][j]);
      
      puzzle.dlxSolve();
      
      // reset data structure
      for (int j = args.parallelLevel; j > 0; j--)
        puzzle.leaveBranch(removedRowCount[j-1]);
      
      numSolution += puzzle.numSolution;
      
      if (args.info) {
        #pragma omp critical
        std::cout << "thread "<<omp_get_thread_num()<<" solve in " << tm1.getRunTime() << "ms" << std::endl;
      }
    }
  }
  std::cout << "number of solutions = " << numSolution << '\n';
  std::cout << "solve time=" << tm1.getRunTime() << "ms"<<std::endl;
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
  std::cout << std::fixed;
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
