#include "funcs.h"
#include "config.h"
#include "term.h"
#include <array>
#include <bits/stdint-uintn.h>
#include <cstdint>
#include <cstdio>
#include <exception>
#include <fstream>
#include <iostream>
#include <ostream>
#include <random>
#include <stack>
#include <stdexcept>
#include <string>
#include <thread>
#include <unistd.h>
typedef std::uint64_t line;
typedef std::uint16_t addr;
namespace ym {

std::array<std::uint8_t, 16> regs;
std::array<line, 32> disp;
std::array<bool, 256> keys;
std::stack<addr> callstack;
std::uint16_t PC = 0;
std::uint16_t I = 0;
bool hold = false;

std::fstream *romptr;

std::random_device rdev;
std::default_random_engine reng(rdev());
std::uniform_int_distribution<std::uint8_t> rand8;

char timer = 0;
char sound = 0;
bool keep_alive = true;
std::thread th;

std::map<char, char> keymap {
    {'0', 0x00}, {'1', 0x01}, {'2', 0x02}, {'3', 0x03},
    {'4', 0x04}, {'5', 0x05}, {'6', 0x06}, {'7', 0x07},
    {'8', 0x08}, {'9', 0x09}, {'a', 0x0a}, {'b', 0x0b},
    {'c', 0x0c}, {'d', 0x0d}, {'e', 0x0e}, {'f', 0x0f},
};
  
  void set_keymap(std::map<char, char> const &km) {
    keymap = km;
  }

void timer_thread(char *t, char *s) {
  while (keep_alive) {
    if (*t > 0)
      *t -= 1;
    if (*s > 0) {
      *s -= 1;
      std::cout << "\a";
    }
    usleep(16667);
  }
}

void share_rom(std::fstream *r) { romptr = r; };
void start_timer_thread() { th = std::thread(timer_thread, &timer, &sound); };
void stop_timer_thread() {
  keep_alive = false;
  if (th.joinable()) {
    th.join();
  }
};


  line rotate_right(line l, uint8_t n){
    line sr = l >> n;
    line sl = l << (64-n);
    return sr|sl;
  }
  
inline void printAt(int x, int y, std::string c) {
  std::cout << "\x1B[" << y << ";" << x << "H" << c;
}
  void clearDisplay() {
    std::cout << "\x1B[2J";
  }

void printRegs() {
  int x = 1;
  int y = 35;
  for (int j = 0; j < 2; j++) {
    for (int i = 0; i < 8; i++) {
      int ii = j * 8 + i;
      char s[8];
      std::sprintf(s, "%.1x: %.2x  ", ii, regs[ii]);
      printAt(x + i * 8, y + j, s);
    }
  }
  char s[8];
  std::sprintf(s, "p: %.3x", PC);
  printAt(65, 35, s);
  std::sprintf(s, "i: %.3x", I);
  printAt(65, 36, s);
  if (hold)
    printAt(71, 36, "[ HOLD ]");
  else
    printAt(71, 36, "        ");
}

void clear() {
  printAt(X-1, Y-1, DR);
  for (int i = 0; i < WIDTH; i++)
    printAt(X+i, Y-1, HH);
  printAt(X+WIDTH, Y-1, DL);
  for (int j = Y; j < HEIGHT + Y; j++) {
    printAt(X-1, j, VV);
    for (int i = X; i < WIDTH + X; i++) {
      printAt(i, j, " ");
    }
    printAt(X+WIDTH, j, VV);
  }
  printAt(X-1, HEIGHT + Y, UR);
  for (int i = X; i < WIDTH+X; i++)
    printAt(i, HEIGHT + Y, HH);
  printAt(WIDTH + X, HEIGHT + Y, UL);
  disp.fill(0);
}

/*
void desp_flip() {
for (int i=0;i<32;i++) {
  if (disp[i] == disp_buff[i]) continue;
  line d = disp[i];
  line b = disp_buff[i];
  for (int ii=0; ii<64; ii++){

  }
}
};
*/
inline void regdebug() {
  if (DEBUG_LVL > 0) {
    printRegs();
  }
};
void update_line(line y) {
  line l = disp[y];
  std::string s = "";
  for (int i = 0; i < 64; i++) {
    if (l % 2)
      s = ONPIXEL + s;
    else
      s = OFFPIXEL + s;
    l >>= 1;
  }
  printAt(X, Y + y, s);
};
void flip_8pixel(line x, line y, char data) {
  line l = disp[y];
  line mask = (line)data << 56;
  mask = rotate_right(mask, x); 

  if (disp[y] & mask) {
    regs[15] = 1;
    regdebug();
  }
  disp[y] ^= mask;
  update_line(y);
}

void cursor_rest() { printAt(CURSOR_REST_X, CURSOR_REST_Y, ""); }
////////////////////// instructions implementation;
void CLR(op o) { ym::clear(); }
void RET(op o) {
  addr n = callstack.top();
  callstack.pop();
  if (DEBUG_LVL > 2) {
    char s[10];
    std::sprintf(s, "%.4x", n);
    printAt(67, 1, s);
  }
  romptr->seekg(n);
}
void JMP(op o) {
  op n = o & 0x0fff;
  romptr->seekg(n);
}
void CALL(op o) {
  callstack.push((addr)romptr->tellg());
  JMP(o);
};
void XEQN(op o) {
  char n = o & 0xff;
  o >>= 8;
  char x = o & 0xf;
  if (regs[x] == n) {
    romptr->seekg(2, std::fstream::cur);
  }
}
void XNEQN(op o) {
  char n = o & 0xff;
  o >>= 8;
  char x = o & 0xf;
  if (regs[x] != n) {
    romptr->seekg(2, std::fstream::cur);
  }
}

void XEQY(op o) {
  o >>= 4;
  char y = o & 0xf;
  o >>= 4;
  char x = o & 0xf;
  if (regs[x] == regs[y]) {
    romptr->seekg(2, std::fstream::cur);
  }
}
void XNEQY(op o) {
  o >>= 4;
  char y = o & 0xf;
  o >>= 4;
  char x = o & 0xf;
  if (regs[x] != regs[y]) {
    romptr->seekg(2, std::fstream::cur);
  }
}
void XSETN(op o) {
  char n = o;
  o >>= 8;
  char r = o & 0xf;
  regs[r] = n;
  regdebug();
}
void XINCN(op o) {
  char n = o;
  o >>= 8;
  char r = o & 0xf;
  regs[r] += n;
  regdebug();
}
void XSETY(op o) {
  char op = o & 0xf;
  o >>= 4;
  char y = o & 0xf;
  o >>= 4;
  char x = o & 0xf;
  switch (op) {
  case 0:
    regs[x] = regs[y];
    break;
  case 1:
    regs[x] |= regs[y];
    break;
  case 2:
    regs[x] &= regs[y];
    break;
  case 3:
    regs[x] ^= regs[y];
    break;
  case 4:
    regs[15] = regs[x] > (255 - regs[y]);
    regs[x] += regs[y];
    break;
  case 5:
    regs[15] = regs[x] >= regs[y];
    regs[x] -= regs[y];
    break;
  case 6:
    regs[15] = regs[x] & 1;
    regs[x] >>= 1;
    break;
  case 7:
    regs[15] = regs[y] >= regs[x];
    regs[x] = regs[y] - regs[x];
    break;
  case 14:
    regs[15] = regs[x] >> 7;
    regs[x] <<= 1;
    break;
  }
  regdebug();
}
void MEM(op o) {
  std::uint16_t nnn = o & 0x0fff;
  I = nnn;
  regdebug();
}
void JUMPRV0(op o) {
  std::uint16_t nnn = o & 0x0fff;
  romptr->seekg(nnn + regs[0]);
}
void RAND(op o) {
  char n = o & 0xff;
  char x = (o >> 8) & 0xf;
  regs[x] = rand8(reng) & o;
  regdebug();
}
void DRAW(op o) {
  char n = o & 0xf;
  o >>= 4;
  char y = o & 0xf;
  o >>= 4;
  char x = o & 0xf;
  auto save = romptr->tellg();
  romptr->seekg(I);

  regs[15] = 0;
  
  for (int j = 0; j < n; j++) {
    line yp = regs[y] % 32 + j;
    if (yp >= 32) break;
    char data[1];
    romptr->read(data, 1);
    flip_8pixel(regs[x] % 64, yp, *data);
  }
  romptr->seekg(save);
} // ym::clear(); }

void TST(op o) {
  char x = (o >> 8) & 0xf;
  timer = regs[x];
};
void TGT(op o) {
  char x = (o >> 8) & 0xf;
  regs[x] = timer;
  regdebug();
}
void SST(op o) {
  char x = (o >> 8) & 0xf;
  sound = regs[x];
};

bool tryKeyMatch(char r) {
  if (keys[r]) {
    keys[r] = false;
    return true;
  };
  char c[1];
  int f = read(STDIN_FILENO, c, 1);
  if (f) {
    try {
      c[0] = keymap.at(c[0]);
    } catch (std::out_of_range) {
    }
    if (c[0] == r) {
      // regs[r] = false;
      return true;
    } else
      keys[c[0]] = true;
  }
  return false;
}

void KEQX(op o) {
  char x = (o >> 8) & 0xf;
  if (tryKeyMatch(regs[x]))
    romptr->seekg(2, std::fstream::cur);
}
void KNEQX(op o) {
  char x = (o >> 8) & 0xf;
  if (!tryKeyMatch(regs[x]))
    romptr->seekg(2, std::fstream::cur);
}
void KGT(op o) {

  char x = (o >> 8) & 0xf;
  char c[1];
  hold = true;
  regdebug();
  ym::btermIn();
  int r = read(STDIN_FILENO, c, 1);
  ym::ntermIn();
  if (r) {
    try {
      c[0] = keymap.at(c[0]);
    } catch (std::out_of_range) {
    }

    regs[x] = c[0];
  }
  hold = false;
  regdebug();
}
void IINCX(op o) {
  char x = (o >> 8) & 0xf;
  I += regs[x];
  regdebug();
};
void LDFNT(op o) {
  char x = (o >> 8) & 0xf;
  I = regs[x] * 5;
  regdebug();
}
void BCD(op o) {
  char x = (o >> 8) & 0xf;
  char r = regs[x];
  char c = r % 10;
  r /= 10;
  char b = r % 10;
  char a = r / 10;
  auto save = romptr->tellg();
  romptr->seekp(I);
  romptr->put(a);
  romptr->seekg(I + 1);
  romptr->put(b);
  romptr->seekg(I + 2);
  romptr->put(c);
  romptr->seekg(save);
}
void REGDUMP(op o) {
  char x = (o >> 8) & 0xf;
  auto save = romptr->tellg();
  romptr->seekp(I);
  for (int i = 0; i <= x; i++) {
    romptr->put(regs[i]);
  };
  romptr->seekg(save);
};
void REGLOAD(op o) {
  char x = (o >> 8) & 0xf;
  auto save = romptr->tellg();
  romptr->seekg(I);
  for (int i = 0; i <= x; i++) {
    char t[1];
    romptr->read(t, 1);
    regs[i] = *t;
  }
  romptr->seekg(save);
  regdebug();
}
std::fstream &readopc(std::fstream &i, op &o) {
  char c1;
  i.read(&c1, 1);
  char c2;
  i.read(&c2, 1);
  o = c1;
  o <<= 8;
  o |= c2;
  if (DEBUG_LVL >1) {
    char s[12];
    int pos = (int)i.tellg() - 2;
    std::sprintf(s, "@%.4x: %.4x", pos,o);
      
      for (int i=1; i<33;++i) {
	printAt(71, i, " ");
      }
      printAt(71, pos/2%32 + 1, s);
  }
  return i;
}

} // namespace ym
