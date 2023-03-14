#ifndef _symbolTable_h_
#define _symbolTable_h_
#include <string>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <bitset>

using namespace std;

struct UsageListSlot {
    string section;
    int address;
    int type;
    UsageListSlot(string sec, int addr, int t) { section = sec; address = addr; type = t; }
};

struct RowOfSymbolTable{
  string SymbolName;
  int SectionNumber;
  int value;
  bool isGlobal;
  int number;
  int size;
  vector<UsageListSlot> usageList;
  vector<char> code;
  RowOfSymbolTable(string SymNm, int SecNum, int val, bool isG, int num, int sz){
    SymbolName = SymNm;
    SectionNumber = SecNum;
    value = val;
    isGlobal = isG;
    number = num;
    size = sz;
  }
};

class SymbolTable{
public:
  SymbolTable();
  void addRow(string SymNm, int SecNum, int val, bool isG, int sz);
  bool containsSymbol(string sym);
  void changeGlobal(string sym);
  void changeSize(string sym, int sz);
  void changeValue(string sym, int val, string sec);
  void addUsageListSlot(string sym,string sec, int addr, int t);
  void insertCode(string sec, char code, int secCnt);
  void replaceCode(string sec, int addr, int code);
  void resolveAndGenerate(vector<string> externSymbols);
  int getSize() { return rows.size(); }
  RowOfSymbolTable getRowAt(int i);
  RowOfSymbolTable getRowWithName(string sym);
  int getSizeAt(int i);
  string getSymNameAt(int i);
  void fillCodeWithZeros(int ind, int addr);

  void writeToFile(string output);
  void writeCodeToFileAt(string output, string section);

  bool isExtern(string sym, vector<string> externSymbols);
 

  static int numberCounter;
private:
  vector<RowOfSymbolTable> rows;
};

#endif
