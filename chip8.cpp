#include "funcs.h"
#include "term.h"
#include <array>
#include "options.h"
//#include <fstream>
#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <ios>
#include <iostream>
//#include <string>
#include <cstdio>
#include <sstream>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
typedef std::uint16_t op;

struct Opc {
  op mask;
  op value;
  void (*func)(op);
};


#define NUM_opc 26
std::array<Opc, 37> opcodes = {
    Opc{0xffff, 0x00e0, ym::CLR},     Opc{0xffff, 0x00ee, ym::RET},
    Opc{0xf000, 0x1000, ym::JMP},     Opc{0xf000, 0x2000, ym::CALL},
    Opc{0xf000, 0x3000, ym::XEQN},    Opc{0xf000, 0x4000, ym::XNEQN},
    Opc{0xf00f, 0x5000, ym::XEQY},    Opc{0xf000, 0x6000, ym::XSETN},
    Opc{0xf000, 0x7000, ym::XINCN},   Opc{0xf000, 0x8000, ym::XSETY},
    Opc{0xf000, 0x9000, ym::XNEQY},   Opc{0xf000, 0xa000, ym::MEM},
    Opc{0xf000, 0xb000, ym::JUMPRV0}, Opc{0xf000, 0xc000, ym::RAND},
    Opc{0xf000, 0xd000, ym::DRAW},    Opc{0xf0ff, 0xe09e, ym::KEQX},
    Opc{0xf0ff, 0xe0a1, ym::KNEQX},   Opc{0xf0ff, 0xf007, ym::TGT},
    Opc{0xf0ff, 0xf00a, ym::KGT},     Opc{0xf0ff, 0xf015, ym::TST},
    Opc{0xf0ff, 0xf018, ym::SST},     Opc{0xf0ff, 0xf01e, ym::IINCX},
    Opc{0xf0ff, 0xf029, ym::LDFNT},   Opc{0xf0ff, 0xf033, ym::BCD},
    Opc{0xf0ff, 0xf055, ym::REGDUMP}, Opc{0xf0ff, 0xf065, ym::REGLOAD},
};

// std::ifstream rom;
options m_option;

void cleanup() {
  ym::stop_timer_thread();
  ym::otermIn();
  if (m_option.clear) {
    ym::clearDisplay();
  }
}

void on_interupt(int s) {
  cleanup();
  exit(s);
};
void set_sigaction() {
  struct sigaction sa;
  sa.sa_handler = on_interupt;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGINT, &sa, NULL);
}

void check_file_open_error(std::ios const *f, char const* msg = "") {
  if (f->fail()) {
    int e = errno;
    cleanup();
    std::cout << msg << ": " << strerror(e);
    exit(e);
      }
};
extern char _binary_interpreter_rom_rom_start[];
extern const char _binary_interpreter_rom_rom_size;
size_t rom_size = (uintptr_t)&_binary_interpreter_rom_rom_size;
std::string_view irom(_binary_interpreter_rom_rom_start,rom_size);
int DEBUG_LVL = m_option.debug_lvl;
int main(int argc, char *argv[]) {
  get_options(argc, argv, &m_option);
  if (m_option.bufsize > 0) {
    char *buf = (char *)malloc(m_option.bufsize);
    int s = setvbuf(stdout, buf, _IOFBF, m_option.bufsize);
    if (s) std::perror("While setteng buffer");
  } else {
    int s = setvbuf(stdout, NULL, _IONBF, 0);
    if (s) std::perror("While setteng buffer");
  }
    if (!m_option.keymap.empty()) {
    ym::set_keymap(m_option.keymap);
  }
  DEBUG_LVL = m_option.debug_lvl;
  set_sigaction();
  ym::term_init();
  ym::ntermIn();
  std::ifstream rom(m_option.filename, std::ios::binary);
  check_file_open_error(&rom,m_option.filename);
  std::stringstream temp{};
  temp << irom << rom.rdbuf();
  temp.seekg(0x200);
  temp.seekp(0x200);
  rom.close();
  ym::share_rom(&temp);
  ym::start_timer_thread();
  ym::clear();
  while (true) {
    op o;
    ym::readopc(temp, o);
    if (!o)
      break;
    for (int idx = 0; idx < NUM_opc; idx++) {
      Opc i = opcodes[idx];
      if ((o & i.mask) == i.value) {
        i.func(o);
        break;
      }

      // printf("%.4x", o.value);
    }
    if (m_option.fps)
    usleep(1000000/m_option.fps);
  }

  // char a[1];
  // rom.read(a,1);
  // rom.read(a,2);;
  // printf("%.2x",a[1]);
  // op b =(op) *a;
  // rom >> a;

  // std::cout << a;
  // ym::clear();

  cleanup();
  return 0;
}
