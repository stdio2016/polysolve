#include "CmdArgs.hpp"
#include <cstring>
#include <iostream>
#include <exception>

void CmdArgs::setDefault(void) {
  help = false;
  ver = false;
  info = false;
  parallelLevel = 0;
  numThreads = 0;
}

static int mystoi(std::string str, int &out, std::string what, int minV, int maxV) {
  try {
    out = std::stoi(str);
  }
  catch (std::invalid_argument &x) {
    std::cerr << "Error: " + what + " must be a nonnegative integer" << std::endl;
    return false;
  }
  catch (std::out_of_range &x) {
    std::cerr << "Error: " + what + " is out of integer range" << std::endl;
    return false;
  }
  if (out > maxV) {
    std::cerr << "Error: " + what + " must be <= " + std::to_string(maxV) << std::endl;
    return false;
  }
  if (out < minV) {
    std::cerr << "Error: " + what + " must be >= " + std::to_string(minV) << std::endl;
    return false;
  }
  return true;
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
      else if (strcmp(argv[i], "-pl") == 0 || strcmp(argv[i], "--parallel-level") == 0) {
        if (i+1 < argc) {
          i += 1;
          if (!mystoi(argv[i], parallelLevel, "parallel level", 0, 1000))
            return false;
        }
      }
      else if (strcmp(argv[i], "-nt") == 0 || strcmp(argv[i], "--num-threads") == 0) {
        if (i+1 < argc) {
          i += 1;
          if (!mystoi(argv[i], numThreads, "number of threads", 0, 1000))
            return false;
        }
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
