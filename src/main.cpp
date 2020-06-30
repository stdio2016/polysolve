#include <iostream>
#include <fstream>
#include <exception>
#include "CmdArgs.hpp"
#include "PuzzleReader.hpp"

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
  if (args.filename.empty()) {
    args.showUsage(argv[0]);
    return 1;
  }
  
  std::ifstream puzzlefile(args.filename);
  if (!puzzlefile) {
    std::cerr << "Error: File \"" << args.filename << "\" not found."
              << std::endl;
    return 1;
  }
  try {
    PuzzleReader reader;
    Puzzle puzzle = reader.read(puzzlefile);
  }
  catch (std::exception& x) {
    std::cerr << "Error: Cannot parse puzzle file \"" << args.filename << "\"."
              << std::endl;
    std::cerr << x.what() << std::endl;
    return 1;
  }
  return 0;
}
