#include "CmdArgs.hpp"
#include <cstring>
#include <iostream>
#include <exception>

void CmdArgs::setDefault(void) {
  help = false;
  ver = false;
  info = false;
  parallelLevel = 0;
  numThreads = 1;
}

static int mystoi(std::string str, int &out, std::string what, int minV, int maxV) {
  try {
    out = std::stoi(str);
  }
  catch (std::invalid_argument &x) {
    std::cerr << "Error: " + what + " must be an integer" << std::endl;
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
      if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
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
  std::cout << "polysolve Version 0.1" << std::endl;
  std::cout << "Compiled with " <<
#ifdef __NVCC__
    "NVCC " << __CUDACC_VER_MAJOR__ << "." << __CUDACC_VER_MINOR__ << "." << __CUDACC_VER_BUILD__ << " "
#endif

#ifdef __clang__
    "Clang " << __clang_major__ << "." << __clang_minor__ << "." << __clang_patchlevel__;
#elif defined(__GNUC__)
    "GCC " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__;
#elif defined(_MSC_FULL_VER)
    "MSVC " << _MSC_FULL_VER;
#else
    "unknown compiler";
#endif

  std::cout << " for " <<
#ifdef _WIN32
    "Windows"
#elif defined(__linux__)
    "Linux"
#elif defined(__APPLE__)
    "macOS"
#elif defined(__unix__)
    "Unix-like"
#else
    "unknown"
#endif
  << " platform." << std::endl;

#ifdef USE_GPU
  std::cout << "GPU support: TODO" << std::endl;
#else
  std::cout << "GPU support: No" << std::endl;
#endif
}

void printHelp(std::string str) {
  int col = 6, first = true;
  int tab = 6;
  for (int i = 0; i < str.size(); i++) {
    int end = i;
    while (end < str.size() && str[end] > ' ') {
      end += 1;
    }
    std::string word(&str[i], &str[end]);
    if (col + word.size() >= 80) {
      std::cout << std::endl;
      first = true;
      col = tab;
    }
    if (first) {
      first = false;
      std::cout << "      ";
    }
    else std::cout.put(' ');
    std::cout << word;
    col += 1 + word.size();
    i = end;
    if (end < str.size()) {
      if (str[end] == '\n') {
        std::cout << std::endl;
        first = true;
        col = tab = 6;
      }
      if (str[end] == '\t') {
        tab = col;
      }
    }
  }
}

void CmdArgs::showUsage(std::string name) {
  if (name.empty()) name = "./polysolve";
  std::cout << "A program to solve polyomino puzzle with multithreading." << '\n';
  std::cout << '\n';
  std::cout << "Usage: " << name << " [options]" << " puzzlefile"
            << std::endl;
  std::cout << std::endl;
  std::cout << "Options:" << '\n';
  std::cout << "  --help (-h)" << '\n';
  printHelp("Display available options\n");
  std::cout << "  --version" << '\n';
  printHelp("Display version information\n");
  std::cout << "  --info (-i)" << '\n';
  printHelp("Display performance measurements\n");
  std::cout << "  --parallel-level <value> (-pl)" << '\n';
  printHelp("Set the number of pieces to place when generating subproblems. "
    "A setting of 0 will disable multithreading. "
    "The default value is 0.\n");
  std::cout << "  --num-threads <value> (-nt)" << '\n';
  printHelp("Set the number of threads for multi-thread solving. "
    "A setting of 1 disables multithreading, "
    "and a setting of 0 will use all CPU cores in this computer. "
    "The default value is 1.\n");
}
