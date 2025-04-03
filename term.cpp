#include "term.h"
#include <iostream>
#include <unistd.h>

namespace ym {

termios oterm;
termios nterm;
termios bterm;
bool term_inited = false;
void term_init() {
  if (!term_inited) {
    tcgetattr(STDIN_FILENO, &oterm);

    nterm = oterm;
    nterm.c_lflag &= ~(ICANON | ECHO);
    nterm.c_cc[VMIN] = 0;
    nterm.c_cc[VTIME] = 0;

    bterm = nterm;
    bterm.c_cc[VMIN] = 1;

    term_inited = true;
  }
}
void ntermIn() {
  tcsetattr(STDIN_FILENO, TCSANOW, &nterm);
  std::cout << "\x1b[?25l"; //hide cursor;
  }
void btermIn() { tcsetattr(STDIN_FILENO, TCSANOW, &bterm); }
void otermIn() {
  tcsetattr(STDIN_FILENO, TCSANOW, &nterm);
  std::cout << "\x1b[?25h"; //show cursor
}

} // namespace ym
