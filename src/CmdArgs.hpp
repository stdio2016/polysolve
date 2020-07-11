/**
 * Parse command lines, and show help/version/usage info.
 */
#pragma once
#ifndef ARGUMENT_PARSER_HPP
#define ARGUMENT_PARSER_HPP
#include <string>
#include <vector>

class CmdArgs {
public:
  std::vector<std::string> filenames;
  bool help;
  bool ver;
  int info;
  int parallelLevel;
  int numThreads;
  
  void setDefault(void);
  bool parseCmdLine(int argc, char *argv[]);
  void showHelp(void);
  void showVersion(void);
  void showUsage(std::string name);
};

#endif
