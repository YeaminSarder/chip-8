#include "options.h"
#include <cstring>
#include <getopt.h>
#include <iostream>

const char *help_message =
R"(Usage: chip8 [OPTIONS] ROM
Emulate the chip8 ROM

  -d, --debug-level=LEVEL  set debug level to LEVEL. default 1.
                           0  -> disable
                           >0 -> show registers
                           >2 -> show callstack on return
  -f, --fps=NUM            limits the framerate of emulation to NUM; default 60.
                           set to 0 for no framerate limit.
  -h, --help               show this help
  -m, --inputmap=MAP       MAP must be exactly 16 characters. default 0123456789abcdef
                           You can use these characters as key intead of 0x0-0xf
)";


const struct option longopts[] = {
    {.name = "debug-level",
     .has_arg = required_argument,
     .flag = NULL,
     .val = 'd'},
    {.name = "fps", .has_arg = required_argument, .flag = NULL, .val = 'f'},
    {.name = "help", .has_arg = no_argument, .flag = NULL, .val = 'h'},
    {.name = "inputmap", .has_arg = required_argument, .flag = NULL, .val = 'm'},
    {} //last element must be all zeros
};



void get_options(int argc, char *argv[], options *op) {

  while (true) {
    int o = getopt_long(argc, argv, "hd:f:m:", longopts, NULL);
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
    case 'm':
      if (strlen(optarg) != 16) {
	std::cerr << "MAP must be exactly 16 characters long";
	exit(1);
      }
      op->keymap.clear();
      for (int i=0; i < 16; ++i) {
	op->keymap.emplace(optarg[i],i);
      }
      break;
    case 'h':
    case '?':
      std::cerr << help_message;
      exit(1);
    }
  }
  if (optind < argc) {
    op->filename = argv[optind];
  } else {
    std::cerr << "./chip8: ROM is not optional.\n";
    std::cerr << help_message;
    exit(1);
  }
}
