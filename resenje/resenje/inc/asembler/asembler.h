#ifndef _asembler_h
#define _asembler_h

#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include "symbolTable.h"
#include "relocationTable.h"

using namespace std;

class Asembler{
public:

  Asembler(const char* inputFile, const char* outputFile);
  void start();

private:

  int end;

  bool isComm;
  bool isLabel;

  static int sectionCounter;
  string currentSection;

  string input;
  string output;
  vector<string> globalSymbols;
  vector<string> externSymbols;
  vector<string> comments;

  vector<char> code;
  SymbolTable* symbolTable;
  vector<RelocationTable> relocationTables;

  void lineread(string line);
  void resolveAndGenerate();
  void makeRelocationTable();
  void makeOutFile();
  //direktive
  void lineIsGlobal(string line);
  void lineIsExtern(string line);
  void lineIsSection(string line);
  void lineIsWord(string line);
  void lineIsSkip(string line);
  void lineIsEnd();
  //instrukcije
  void lineIsHalt(string line);
  void lineIsInt(string line);
  void lineIsIret(string line);
  void lineIsCall(string line);
  void lineIsRet(string line);
  void lineIsJmp(string line);
  void lineIsJeq(string line);
  void lineIsJne(string line);
  void lineIsJgt(string line);
  void lineIsPush(string line);
  void lineIsPop(string line);
  void lineIsXchg(string line);
  void lineIsAdd(string line);
  void lineIsSub(string line);
  void lineIsMul(string line);
  void lineIsDiv(string line);
  void lineIsCmp(string line);
  void lineIsNot(string line);
  void lineIsAnd(string line);
  void lineIsOr(string line);
  void lineIsXor(string line);
  void lineIsTest(string line);
  void lineIsShl(string line);
  void lineIsShr(string line);
  void lineIsLdr(string line, int num);
  void lineIsStr(string line, int num);

  void twoRegInstructions(string line, int instrDesc, int cutoff);
  void JumpInstructions(string line, int InstrDesc, int cutoff);
  void dataInstrctions(string line, int InstrDesc, int ins);

  string cutOffStartBlanks(string line);
  int hexToInt(string hex);
};


#endif




