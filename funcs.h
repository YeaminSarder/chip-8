#pragma once
#include <cstdint>
#include <map>
#include <string>
#include <fstream>
#include <thread>
//extern std::ifstream rom; 
typedef std::uint16_t op;
namespace ym {
  void share_rom(std::fstream* rom);
  void set_keymap(std::map<char, char> const &km);
  void start_timer_thread();
  void stop_timer_thread();
  inline void printAt(int x, int y, std::string c);
  void clear();
  void clearDisplay();
  void cursor_rest();
  void CLR(op o);
  void RET(op o);
  void JMP(op o);
  void CALL(op o);
  void XEQN(op o);
  void XNEQN(op o);
  void XEQY(op o);
  void XSETN(op o);
  void XINCN(op o);
  void XSETY(op o);
  void XNEQY(op o);
  void MEM(op o);
  void JUMPRV0(op o);
  void RAND(op o);
  void DRAW(op o);
  void TST(op o);
  void KEQX(op o);
  void KNEQX(op o);
  void KGT(op o);
  void TGT(op o);
  void SST(op o);
  void IINCX(op o);
  void LDFNT(op o);
  void BCD(op o);
  void REGDUMP(op o);
  void REGLOAD(op o);

 std::fstream &readopc(std::fstream &i, op &o);
}
