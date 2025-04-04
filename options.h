typedef struct options {
  int debug_lvl = 1;
  int fps = 60;
  char* filename;
} options;
void get_options(int argc, char* argv[], options *op);
