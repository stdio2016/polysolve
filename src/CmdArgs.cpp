#include "CmdArgs.hpp"
#include <cstring>
#include <iostream>

void CmdArgs::setDefault(void) {
  filename = std::string();
  help = false;
  ver = false;
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
      else if (strcmp(argv[i], "--") == 0) {
        // this is not a command line option
        if (i+1 < argc) {
          i += 1;
          filename = argv[i];
        }
      }
      else {
        std::cerr << "Error: Unknown option: " << argv[i] << std::endl;
        return false;
      }
    }
    else {
      filename = argv[i];
    }
  }
  return true;
}

void CmdArgs::showHelp(void) {
  std::cout << "TODO" << std::endl;
}

void CmdArgs::showVersion(void) {
  std::cout << "blocksolve Version 0.0" << std::endl;
}

void CmdArgs::showUsage(std::string name) {
  if (name.empty()) name = "./polysolve";
  std::cerr << "Usage: " << name << " [options]" << " puzzlefile"
            << std::endl;
  std::cerr << "Get more help with --help option." << std::endl;
}
