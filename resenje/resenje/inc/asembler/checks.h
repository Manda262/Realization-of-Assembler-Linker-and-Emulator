#ifndef _checks_h_
#define _checks_h_

#include <regex>
#include <string>
#include "asembler.h"
using namespace std;

class Checks{
public:

  static bool checkArgs(const char* option,const char * input,const  char *  output);
  static void checksArgsLength(int argc, char* argv[]);
  static string findNextSymbol(string line);
  static int checkEmpty(string line);
  static int checkComments(string line);
  static int checkLabel(string line);
  static int checkDir(string line);
  static int checkInstruction(string line);
  static int getOneRegFromLine(int cutoff, string line);
  static string getOperandFromLine(int cutoff, string line);
  static string getOperandFromOperand(string line);
  static string getDirectiveFromLine(string line);

  static int checkIfTableExists(vector<RelocationTable> tables, string section);
  static int findRelocationTableWithThisSection(vector<RelocationTable> tables, string section);
};

#endif
