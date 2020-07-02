#include "CmdArgs.hpp"
#include <cstring>
#include <iostream>

void CmdArgs::setDefault(void) {
  help = false;
  ver = false;
  info = false;
}

bool CmdArgs::parseCmdLine(int argc, char *argv[]) {
  this->setDefault();
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      if (strcmp(argv[i], "--help") == 0) {
        help = true;
      }
      else if (strcmp(argv[i], "--version") == 0) {
        ver = true;
      }
      else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--info") == 0) {
        info = true;
      }
      else if (strcmp(argv[i], "--") == 0) {
        // this is not a command line option
        if (i+1 < argc) {
          i += 1;
          filenames.push_back(argv[i]);
        }
      }
      else {
        std::cerr << "Error: Unknown option: " << argv[i] << std::endl;
        return false;
      }
    }
    else {
      filenames.push_back(argv[i]);
    }
  }
  return true;
}

void CmdArgs::showHelp(void) {
  showUsage("");
}

void CmdArgs::showVersion(void) {
  std::cout << "polysolve Version 0.0" << std::endl;
}

void CmdArgs::showUsage(std::string name) {
  if (name.empty()) name = "./polysolve";
  std::cout << "Usage: " << name << " [options]" << " puzzlefile"
            << std::endl;
}
