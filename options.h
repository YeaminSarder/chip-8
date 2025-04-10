#pragma once
#include <map>
typedef struct options {
  int debug_lvl = 1;
  int fps = 60;
  char* filename;
  int bufsize = 0;
  std::map<char,char> keymap = {};
  int clear = 1;
} options;
void get_options(int argc, char* argv[], options *op);
