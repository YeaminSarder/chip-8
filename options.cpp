#include "options.h"
#include <iostream>
#include <bits/getopt_core.h>

void get_options(int argc, char* argv[], options *op) {
  while (true) {
    int o = getopt(argc, argv, "d:f:");
    if (o == -1)
      break;
    switch (o) {
    case 'd':
      try {
        op->debug_lvl = std::stoi(optarg);
      } catch (std::invalid_argument) {
        std::cerr << "Debug level must be an integer";
        exit(1);
      };
      break;
    case 'f':
      try {
        op->fps = std::stoi(optarg);
      } catch (std::invalid_argument) {
        std::cerr << "fps must be an integer";
        exit(1);
      };
      break;
    case '?':
      std::cerr << "help: chip8 [-d debug level] [-f fps] rom";
      exit(1);
    }
  }
  op->filename = argv[optind];
}
