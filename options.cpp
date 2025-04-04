#include "options.h"
#include <getopt.h>
#include <iostream>

const struct option longopts[] = {
    {.name = "debug-level",
     .has_arg = required_argument,
     .flag = NULL,
     .val = 'd'},
    {.name = "fps", .has_arg = required_argument, .flag = NULL, .val = 'f'},
    {.name = "help", .has_arg = no_argument, .flag = NULL, .val = 'h'},
    {} //last element must be all zeros
};

void get_options(int argc, char *argv[], options *op) {

  while (true) {
    int o = getopt_long(argc, argv, "hd:f:", longopts, NULL);
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
    case 'h':
    case '?':
      std::cerr << "help: chip8 [-d debug level] [-f fps] rom";
      exit(1);
    }
  }
  op->filename = argv[optind];
}
