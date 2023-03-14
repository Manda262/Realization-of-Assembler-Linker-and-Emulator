#include "checks.h"
#include <iostream>
#include <stdexcept>


bool Checks::checkArgs(const char * option, const char * input, const char *  output){

  regex inputreg("([^ ^.]+\\.s)");
  regex outputreg("([^ ^.]+\\.o)");
  regex optionreg("-o");

  if(regex_match(option, optionreg) && regex_match(input, inputreg) && regex_match(output, outputreg)){
    return true;
  }else{
    return false;
  }
}

void Checks::checksArgsLength(int argc, char* argv[]){

  bool b = false;
  if(argc == 3){
    b = Checks::checkArgs("-o" ,argv[2], "random.o");
  }else if(argc == 4){
    b = Checks::checkArgs(argv[1], argv[3], argv[2]);
  }
  if(!b) throw invalid_argument("Argumenti nisu validni!");
}

string Checks::findNextSymbol(string line)
{
    int pos = line.find(" ");
    string substr = line;
    while (pos == 0) {
        substr = substr.substr(pos + 1);
        pos = substr.find(" ");
    }
    pos = substr.find(",");
    if (pos == -1) { 
        int pos1 = substr.find(' ');
        if (pos1 != -1) substr = substr.substr(0, pos1);
        return substr;
    }
    else {
        int pos1 = substr.find(' ');
        if (pos1 != -1 && pos1 < pos) return substr.substr(0, pos1);
        else return substr.substr(0, pos);
    }
}

int Checks::checkEmpty(string line)
{
    for (int i = 0; i < line.size(); i++) {
        if (line.at(i) != ' ') return 0; 
    }
    return 1; 
}

int Checks::checkComments(string line)
{
    return line.find('#');
    
}

int Checks::checkLabel(string line)
{
    return line.find(':');
}

int Checks::checkDir(string line){
  regex glob("(\\.global .+( ?)+((,( ?)+.+)?)+( ?)+)");
  regex ext("(\\.extern .+( ?)+((,( ?)+.+)?)+( ?)+)");
  regex sec("(\\.section .+( ?)+( ?)+)");
  regex w("(\\.word .+( ?)+((,( ?)+.+)?)+( ?)+)");
  regex sk("(\\.skip [0-9]+( ?)+)");
  regex e("(\\.end( ?)+( ?)+)");
  if(regex_match(line,glob)) return 1; 
  if (regex_match(line, ext)) return 2; 
  if (regex_match(line, sec)) return 3;
  if (regex_match(line, w)) return 4; 
  if (regex_match(line, sk)) return 5;
  if (regex_match(line, e)) return 6;
  return 0;
}

int Checks::checkInstruction(string line)
{
    regex halt("(halt( ?)+)");
    regex in("(int ( ?)+r[0-7]( ?)+)");
    regex iret("(iret( ?)+)");
    regex call("(call .+)");
    regex ret("(ret( ?)+)");
    regex jmp("(jmp .+)");
    regex jeq("(jeq .+)");
    regex jne("(jne .+)");
    regex jgt("(jgt .+)");
    regex push("(push ( ?)+((r[0-7])|psw)( ?)+)");
    regex pop("(pop ( ?)+((r[0-7])|psw)( ?)+)");
    regex xchg("(xchg ( ?)+((r[0-7])|psw)( ?)+,( ?)+((r[0-7])|psw)( ?)+)");
    regex add("(add ( ?)+((r[0-7])|psw)( ?)+,( ?)+((r[0-7])|psw)( ?)+)");
    regex sub("(sub ( ?)+((r[0-7])|psw)( ?)+,( ?)+((r[0-7])|psw)( ?)+)");
    regex mul("(mul ( ?)+((r[0-7])|psw)( ?)+,( ?)+((r[0-7])|psw)( ?)+)");
    regex div("(div ( ?)+((r[0-7])|psw)( ?)+,( ?)+((r[0-7])|psw)( ?)+)");
    regex cmp("(cmp ( ?)+((r[0-7])|psw)( ?)+,( ?)+((r[0-7])|psw)( ?)+)");
    regex no("(not ( ?)+((r[0-7])|psw)( ?)+)");
    regex an("(and ( ?)+((r[0-7])|psw)( ?)+,( ?)+((r[0-7])|psw)( ?)+)");
    regex o("(or ( ?)+((r[0-7])|psw)( ?)+,( ?)+((r[0-7])|psw)( ?)+)");
    regex xo("(xor ( ?)+((r[0-7])|psw)( ?)+,( ?)+((r[0-7])|psw)( ?)+)");
    regex test("(test ( ?)+((r[0-7])|psw)( ?)+,( ?)+((r[0-7])|psw)( ?)+)");
    regex shl("(shl ( ?)+((r[0-7])|psw)( ?)+,( ?)+((r[0-7])|psw)( ?)+)");
    regex shr("(shr ( ?)+((r[0-7])|psw)( ?)+,( ?)+((r[0-7])|psw)( ?)+)");
    regex ldr("(ldr ( ?)+((r[0-7])|psw)( ?)+,.+)");
    regex str("(str ( ?)+((r[0-7])|psw)( ?)+,.+)");

    if (regex_match(line, halt)) return 1; // halt
    if (regex_match(line, in)) return 2; // int regD
    if (regex_match(line, iret)) return 3; // iret
    if (regex_match(line, call)) return 4; // call operand
    if (regex_match(line, ret)) return 5; // ret
    if (regex_match(line, jmp)) return 6; // jmp operand
    if (regex_match(line, jeq)) return 7; //jeq operand
    if (regex_match(line, jne)) return 8; // jne operand
    if (regex_match(line, jgt)) return 9; // jgt operand
    if (regex_match(line, push)) return 10; // push regD
    if (regex_match(line, pop)) return 11; // pop regD
    if (regex_match(line, xchg)) return 12; // xchg regD, regS
    if (regex_match(line, add)) return 13; // add regD, regS
    if (regex_match(line, sub)) return 14; // sub regD, regS
    if (regex_match(line, mul)) return 15; // mul regD, regS
    if (regex_match(line, div)) return 16; // div regD, regS
    if (regex_match(line, cmp)) return 17; // cmp regD, regS
    if (regex_match(line, no)) return 18; // not regD
    if (regex_match(line, an)) return 19; // and regD, regS
    if (regex_match(line, o)) return 20; // or regD, regS
    if (regex_match(line, xo)) return 21; // xor regD, regS
    if (regex_match(line, test)) return 22; // test regD, regS
    if (regex_match(line, shl)) return 23; // shl regD, regS
    if (regex_match(line, shr)) return 24; // shr regD, regS
    if (regex_match(line, ldr)) return 25; // ldr regD, regS
    if (regex_match(line, str)) return 26; // str regD, regS
    return 0;
}

int Checks::getOneRegFromLine(int cutoff, string line)
{
    string reg = line.substr(cutoff);
    int i;
    for (i = 0; i < reg.size(); i++) {
        if (reg.at(i) != ' ') break;
    }
    if (regex_match(reg, regex("(( ?)+psw( ?)+)"))) return 8;
    reg = reg.substr(i + 1, 1);
    return stoi(reg);
    
}

string Checks::getOperandFromLine(int cutoff, string line)
{
    string substr = line.substr(cutoff);
    int i;
    for (i = 0; i < substr.size(); i++) {
        if (substr.at(i) != ' ') break;
    }
    substr = substr.substr(i);
    int j;
    int stop = substr.size();
    for (j = 0; j < substr.size(); j++) {
        if (substr.at(j) != ' ') stop = j;
    }
    substr = substr.substr(0, stop + 1);
    return substr;
}

string Checks::getOperandFromOperand(string line)
{
    int i;
    for (i = 0; i < line.size(); i++) {
        if (line.at(i) != ' ') break;
    }
    string substr = line.substr(i);
    int j;
    for (j = 0; j < substr.size(); j++) {
        if (substr.at(j) == ' ' || substr.at(j) == ']') break;
    }
    substr = substr.substr(0, j);
    return substr;
}

string Checks::getDirectiveFromLine(string line)
{
    return string();
}

int Checks::checkIfTableExists(vector<RelocationTable> tables, string section)
{
    for (int i = 0; i < tables.size(); i++) {
        if (section.compare(tables.at(i).getSection()) != 0) { // isti
            return 1;
        }
    }
    return 0;
}

int Checks::findRelocationTableWithThisSection(vector<RelocationTable> tables, string section)
{
    for (int i = 0; i < tables.size(); i++) {
        if (section.compare(tables.at(i).getSection()) == 0) { // isti
            return i;
        }
    }
    throw invalid_argument("Greska u sistemu");
}

