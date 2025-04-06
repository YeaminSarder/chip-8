#pragma once
#include <map>
typedef struct options {
  int debug_lvl = 1;
  int fps = 60;
  char* filename;
  std::map<char,char> keymap = {};
} options;
void get_options(int argc, char* argv[], options *op);
