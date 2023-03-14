#include "asembler.h"
#include "checks.h"
#include <stdexcept>

int Asembler::sectionCounter = 0;

Asembler::Asembler(const char* inputFile, const char* outputFile){
  input = inputFile;
  output = outputFile;
  currentSection = "";
  end = false;
  symbolTable = new SymbolTable();
}

void Asembler::start(){
  fstream inputFile;
	inputFile.open(this->input);
    if (inputFile.is_open() == false) throw invalid_argument("Argumenti nisu validni!");

  string line;

  while(getline(inputFile, line) && !end){
    lineread(line);
  }
  
  resolveAndGenerate();
  makeRelocationTable();
  makeOutFile();
}

void Asembler::lineread(string line){
    isComm = false;
    isLabel = false;
    if (Checks::checkEmpty(line)) return;
    line = cutOffStartBlanks(line);
    if (Checks::checkComments(line) != -1) {
        isComm = true;
        int pos = line.find('#');
        string comm = line.substr(pos + 1);
        comments.push_back(comm);
        line = line.substr(0, pos);
    }
    if (Checks::checkLabel(line) != -1) { // mora da bude u okviru neke sekcije!!!
        isLabel = true;
        if (currentSection == "") throw invalid_argument("Labela van sekcije");
        string label = line.substr(0, line.find(':'));
        for (int i = 0; i < externSymbols.size(); i++) {
            if (label == externSymbols.at(i)) throw invalid_argument("Multiple symbols");
        }
        line = line.substr(line.find(':') + 1);
        if (symbolTable->containsSymbol(label)) {
            symbolTable->changeValue(label, sectionCounter, currentSection);
        }
        else {
            symbolTable->addRow(label, symbolTable->getRowWithName(currentSection).number, sectionCounter, false, -1);
        }
    }
  int dir = Checks::checkDir(line);
  if(dir != 0){
    switch(dir){
      case 1:
        lineIsGlobal(line);
        break;
      case 2:
        lineIsExtern(line);
        break;
      case 3:
        lineIsSection(line);
        break;
      case 4:
        lineIsWord(line);
        break;
      case 5:
        lineIsSkip(line);
        break;
      case 6:
        lineIsEnd();
        break;
    }
  }else{
      int ins = Checks::checkInstruction(line);
      if (ins != 0) {
          switch (ins) {
          case 1:
              lineIsHalt(line);
              break;
          case 2:
              lineIsInt(line);
              break;
          case 3:
              lineIsIret(line);
              break;
          case 4:
              lineIsCall(line);
              break;
          case 5:
              lineIsRet(line);
              break;
          case 6:
              lineIsJmp(line);
              break;
          case 7:
              lineIsJeq(line);
              break;
          case 8:
              lineIsJne(line);
              break;
          case 9:
              lineIsJgt(line);
              break;
          case 10:
              lineIsPush(line);
              break;
          case 11:
              lineIsPop(line);
              break;
          case 12:
              lineIsXchg(line);
              break;
          case 13:
              lineIsAdd(line);
              break;
          case 14:
              lineIsSub(line);
              break;
          case 15:
              lineIsMul(line);
              break;
          case 16:
              lineIsDiv(line);
              break;
          case 17:
              lineIsCmp(line);
              break;
          case 18:
              lineIsNot(line);
              break;
          case 19:
              lineIsAnd(line);
              break;
          case 20:
              lineIsOr(line);
              break;
          case 21:
              lineIsXor(line);
              break;
          case 22:
              lineIsTest(line);
              break;
          case 23:
              lineIsShl(line);
              break;
          case 24:
              lineIsShr(line);
              break;
          case 25:
              lineIsLdr(line, 0);
              break;
          case 26:
              lineIsStr(line, 0);
              break;
          }
      }
      else {
          if(!isComm && !isLabel) throw invalid_argument("Linija koda neispravna");
      }
  }
}

void Asembler::resolveAndGenerate()
{
    symbolTable->resolveAndGenerate(externSymbols);
}

void Asembler::makeRelocationTable()
{

    //prvo pravim sve relokacione tabele

    for (int i = 0; i < symbolTable->getSize(); i++) {
        if (symbolTable->getSizeAt(i) != -1) {
            string section = symbolTable->getSymNameAt(i);
            relocationTables.push_back(RelocationTable(section));
        }
    }

    //zatim ih popunjavam

    for (int i = 0; i < symbolTable->getSize(); i++) {
        RowOfSymbolTable row = symbolTable->getRowAt(i);
        for (int j = 0; j < row.usageList.size(); j++) {
            UsageListSlot slot = row.usageList.at(j);
            int ind = Checks::findRelocationTableWithThisSection(relocationTables, slot.section);
            if (row.isGlobal) relocationTables.at(ind).addRow(slot.type, slot.address, row.number);
            else relocationTables.at(ind).addRow(slot.type, slot.address, row.SectionNumber);
        }
    }
}

void Asembler::makeOutFile()
{
    fstream outputFile;
    outputFile.open(this->output, fstream::in | fstream::out | fstream::app); // otvori ako postoji napravi ako ne postoji
    if (outputFile.is_open() == false) throw invalid_argument("Argumenti nisu validni!");

    outputFile << "SymbolTable\n";
    outputFile.close();
    symbolTable->writeToFile(output);
    outputFile.open(this->output, fstream::app);
    outputFile << "\n\n";
    outputFile << "Relokacione tabele\n";
    outputFile.close();

    for (int i = 0; i < relocationTables.size(); i++) {
        if (relocationTables.at(i).getSection().compare("UND") == 0) continue;
        outputFile.open(this->output, fstream::app);
        outputFile << "Relokaciona tabela za sekciju: " << relocationTables.at(i).getSection() << "\n";
        outputFile.close();
        relocationTables.at(i).writeToFile(output);
        outputFile.open(this->output, fstream::app);
        outputFile << "kod sekcije:\n";
        outputFile.close();
        symbolTable->writeCodeToFileAt(output, relocationTables.at(i).getSection());
    }
}

void Asembler::lineIsGlobal(string line){


    string sym = line.substr(8);
    int pos = line.find(",");
    string substr = "";
    sym = Checks::findNextSymbol(sym);
    if (pos != -1) {
        sym = sym.substr(0, sym.find(","));
        substr = line.substr(pos + 1);
    }

    while (1) {
        globalSymbols.push_back(sym);

        for (string symbol : externSymbols) {
            if (!symbol.compare(sym)) { // ako je nasao simbol u listi externih
                throw invalid_argument("Uvezeni simbol se ne moze ponovo definisati");
                break;
            }
        }

        if (!symbolTable->containsSymbol(sym)) { // prihvatam global global kao void
            symbolTable->addRow(sym, 0, 0, true, -1);
        }
        else {
            symbolTable->changeGlobal(sym);
        }
        
        if (substr != "") {
            sym = Checks::findNextSymbol(substr);
        }
        else break;
        pos = substr.find(",");
        if (pos != -1) {
            substr = substr.substr(pos + 1);
        }
        else substr = "";
    }
}

void Asembler::lineIsExtern(string line)
{
    string sym = line.substr(8);
    int pos = line.find(",");
    string substr = "";
    sym = Checks::findNextSymbol(sym);
    if (pos != -1) {
        substr = line.substr(pos + 1);
    }

    while (1) {

        for (int i = 0; i < globalSymbols.size(); i++) {
            if(globalSymbols.at(i) == sym) throw invalid_argument("direktiva extern neispravno upotrebljena");
        }

        if (!symbolTable->containsSymbol(sym)) {  // ne prihvatam ni situaciju extern extern
            symbolTable->addRow(sym, 0, 0, true, -1);
        }
        else {
            if(symbolTable->getRowWithName(sym).SectionNumber == 0) symbolTable->changeGlobal(sym);
            else throw invalid_argument("direktiva extern neispravno upotrebljena");
        }

        externSymbols.push_back(sym);

        if (substr != "") {
            sym = Checks::findNextSymbol(substr);
        }
        else break;
        pos = substr.find(",");
        if (pos != -1) {
            substr = substr.substr(pos + 1);
        }
        else substr = "";

    }
}

void Asembler::lineIsSection(string line)
{
    string sym = line.substr(9);
    sym = Checks::findNextSymbol(sym);

    if (!symbolTable->containsSymbol(sym)) {
        symbolTable->addRow(sym, SymbolTable::numberCounter, 0, false, 0);
    }
    else {
        throw invalid_argument("direktiva section neispravno upotrebljena");
    }
    if (currentSection.compare("")) { // nije nedefinisana pocetna sekcija
        symbolTable->changeSize(currentSection, sectionCounter);
    }
    currentSection = sym;
    sectionCounter = 0;
    
}

void Asembler::lineIsWord(string line)
{
    string sym = line.substr(6);
    int pos = line.find(",");
    string substr = "";
    sym = Checks::findNextSymbol(sym);
    if (pos != -1) {
        substr = line.substr(pos + 1);
    }
    while (1) {

        if (!regex_match(sym, regex("[0-9]+( ?)+")) && (!regex_match(sym, regex("0x[0|1|2|3|4|5|6|7|8|9|A|B|C|D|E|F]+( ?)+")))) {// simbol je u pitanju
            if (!symbolTable->containsSymbol(sym)) {
                symbolTable->addRow(sym, 0, 0, false, -1);
            }
            symbolTable->addUsageListSlot(sym, currentSection, sectionCounter, 1);
            symbolTable->insertCode(currentSection, 0b11111111, sectionCounter);
            symbolTable->insertCode(currentSection, 0b11111111, sectionCounter + 1);
        }
        else { // literal
            int op;
            if (!regex_match(sym, regex("[0-9]+( ?)+"))) {
                sym = sym.substr(2);
                op = hexToInt(sym);
            }
            else op = stoi(sym);
            symbolTable->insertCode(currentSection, op & 0b11111111, sectionCounter);
            symbolTable->insertCode(currentSection, op >> 8, sectionCounter + 1);
        }

        sectionCounter += 2;

        if (substr != "") {
            sym = Checks::findNextSymbol(substr);
        }
        else break;
        pos = substr.find(",");
        if (pos != -1) {
            substr = substr.substr(pos + 1);
        }
        else substr = "";
    
    }
}

void Asembler::lineIsSkip(string line)
{
    string strlit = line.substr(6);
    strlit = Checks::findNextSymbol(strlit);
    int lit = stoi(strlit);
    sectionCounter += lit;
    for (int i = 0; i < lit; i++) {
        symbolTable->insertCode(currentSection, 0b00000000, sectionCounter + i);
    }
}

void Asembler::lineIsEnd()
{
    if (currentSection.compare("")) { // nije nedefinisana pocetna sekcija
        symbolTable->changeSize(currentSection, sectionCounter);
    }
    end = true;
}

void Asembler::lineIsHalt(string line)
{
    symbolTable->insertCode(currentSection, 0b00000000, sectionCounter);
    sectionCounter += 1;
}

void Asembler::lineIsInt(string line)
{
    int reg = Checks::getOneRegFromLine(3, line);
    symbolTable->insertCode(currentSection, 0b00010000, sectionCounter);
    symbolTable->insertCode(currentSection, (reg << 4) + 0b1111, sectionCounter + 1);
    sectionCounter += 2;
}

void Asembler::lineIsIret(string line)
{
    symbolTable->insertCode(currentSection, 0b00100000, sectionCounter);
    sectionCounter += 1;
}

void Asembler::lineIsCall(string line)
{
    JumpInstructions(line, 0b00110000, 4);
}

void Asembler::lineIsRet(string line)
{
    symbolTable->insertCode(currentSection, 0b01000000, sectionCounter);
    sectionCounter += 1;
}

void Asembler::lineIsJmp(string line)
{
    JumpInstructions(line, 0b01010000, 3);
}

void Asembler::lineIsJeq(string line)
{
    JumpInstructions(line, 0b01010001, 3);
}

void Asembler::lineIsJne(string line)
{
    JumpInstructions(line, 0b01010010, 3);
}

void Asembler::lineIsJgt(string line)
{
    JumpInstructions(line, 0b01010011, 3);
}

void Asembler::lineIsPush(string line)
{
    int reg = Checks::getOneRegFromLine(4, line);
    string newLine = "";
    if (reg == 8) newLine = "psw";
    else newLine = "r" + to_string(reg);
    newLine = "str " + newLine + ",[r6]";
    lineIsStr(newLine, 1);
}

void Asembler::lineIsPop(string line)
{
    int reg = Checks::getOneRegFromLine(3, line);
    string newLine = "";
    if (reg == 8) newLine = "psw";
    else newLine = "r" + to_string(reg);
    newLine = "ldr " + newLine + ",[r6]";
    lineIsLdr(newLine, 2);
}

void Asembler::lineIsXchg(string line)
{
    twoRegInstructions(line, 0b01100000, 4);
}

void Asembler::lineIsAdd(string line)
{
    twoRegInstructions(line, 0b01110000, 3);
}

void Asembler::lineIsSub(string line)
{
    twoRegInstructions(line, 0b01110001, 3);
}

void Asembler::lineIsMul(string line)
{
    twoRegInstructions(line, 0b01110010, 3);
}

void Asembler::lineIsDiv(string line)
{
    twoRegInstructions(line, 0b01110011, 3);
}

void Asembler::lineIsCmp(string line)
{
    twoRegInstructions(line, 0b01110100, 3);
}

void Asembler::lineIsNot(string line)
{
    int reg = Checks::getOneRegFromLine(3, line);
    symbolTable->insertCode(currentSection, 0b10000000, sectionCounter);
    symbolTable->insertCode(currentSection, (reg << 4) + 0b1111, sectionCounter + 1);
    sectionCounter += 2;
}

void Asembler::lineIsAnd(string line)
{
    twoRegInstructions(line, 0b10000001, 3);
}

void Asembler::lineIsOr(string line)
{
    twoRegInstructions(line, 0b10000010, 2);
}

void Asembler::lineIsXor(string line)
{
    twoRegInstructions(line, 0b10000011, 3);
}

void Asembler::lineIsTest(string line)
{
    twoRegInstructions(line, 0b10000100, 4);
}

void Asembler::lineIsShl(string line)
{
    twoRegInstructions(line, 0b10010000, 3);
}

void Asembler::lineIsShr(string line)
{
    twoRegInstructions(line, 0b10010001, 3);
}

void Asembler::lineIsLdr(string line, int num)
{
    dataInstrctions(line, 0b10100000, num);
}

void Asembler::lineIsStr(string line, int num)
{
    dataInstrctions(line, 0b10110000, num);
}

void Asembler::twoRegInstructions(string line, int instrDesc, int cutoff)
{
    int pos = line.find(",");
    string firsthalf = line.substr(0, pos);
    string secondhalf = line.substr(pos + 1);
    int reg1 = Checks::getOneRegFromLine(cutoff, firsthalf);
    int reg2 = Checks::getOneRegFromLine(0, secondhalf);
    symbolTable->insertCode(currentSection, instrDesc, sectionCounter);
    symbolTable->insertCode(currentSection, (reg1 << 4) + reg2, sectionCounter + 1);
    sectionCounter += 2;
}

void Asembler::JumpInstructions(string line, int InstrDesc, int cutoff)
{
    string operand = Checks::getOperandFromLine(cutoff, line);
    int pos1 = line.find("%");
    int pos2 = line.find("*");
    symbolTable->insertCode(currentSection, InstrDesc, sectionCounter);
    if (pos1 == -1 && pos2 == -1) {
        symbolTable->insertCode(currentSection, 0b11111111, sectionCounter + 1);
        symbolTable->insertCode(currentSection, 0b00000000, sectionCounter + 2);
        if ((!regex_match(operand, regex("[0-9]+( ?)+"))) && (!regex_match(operand, regex("0x[0|1|2|3|4|5|6|7|8|9|A|B|C|D|E|F]+( ?)+")))) { //simbol
            if (!symbolTable->containsSymbol(operand)) {
                symbolTable->addRow(operand, 0, 0, false, -1);
            }
            symbolTable->addUsageListSlot(operand, currentSection, sectionCounter + 3, 1);
            symbolTable->insertCode(currentSection, 0b11111111, sectionCounter + 3);
            symbolTable->insertCode(currentSection, 0b11111111, sectionCounter + 4);
        }
        else {
            int op;
            if (!regex_match(operand, regex("[0-9]+( ?)+"))) {
                operand = operand.substr(2);
                op = hexToInt(operand);
            }
            else op = stoi(operand);
            symbolTable->insertCode(currentSection, (op) & 0b11111111, sectionCounter + 3);
            symbolTable->insertCode(currentSection, (op) >> 8, sectionCounter + 4);
        }
        sectionCounter += 5;
    }
    if (pos1 != -1 && (pos2 == -1 || pos1 < pos2)) { // PC relativno
        operand = operand.substr(1);
        symbolTable->insertCode(currentSection, 0b11110111, sectionCounter + 1);
        symbolTable->insertCode(currentSection, 0b00000101, sectionCounter + 2);
        if (!symbolTable->containsSymbol(operand)) {
            symbolTable->addRow(operand, 0, 0, false, -1);
        }
        symbolTable->addUsageListSlot(operand, currentSection, sectionCounter + 3, 2); // Tip je PC rel
        symbolTable->insertCode(currentSection, 0b11111111, sectionCounter + 3);
        symbolTable->insertCode(currentSection, 0b11111111, sectionCounter + 4);
        sectionCounter += 5;
    }
    if (pos2 != -1 && (pos1 == -1 || pos2 < pos1)) {
        operand = operand.substr(1);
        if (regex_match(operand, regex("((r[0-7])|psw)"))) {
            int r = Checks::getOneRegFromLine(0, operand);
            symbolTable->insertCode(currentSection, 0b11110000 + r, sectionCounter + 1);
            symbolTable->insertCode(currentSection, 0b00000001, sectionCounter + 2);
            sectionCounter += 3;
        }
        else if (regex_match(operand, regex("(\\[( ?)+((r[0-7])|psw)( ?)+\\])"))) {
            int r = Checks::getOneRegFromLine(1, operand);
            symbolTable->insertCode(currentSection, 0b11110000 + r, sectionCounter);
            symbolTable->insertCode(currentSection, 0b00000010, sectionCounter);
            sectionCounter += 3;
        }
        else if (regex_match(operand, regex("(\\[( ?)+((r[0-7])|psw)( ?)+\\+( ?)+.+( ?)+\\])"))) {
            int r = Checks::getOneRegFromLine(1, operand);
            int pos = operand.find("+");
            operand = operand.substr(pos + 1);
            operand = Checks::getOperandFromOperand(operand);
            symbolTable->insertCode(currentSection, 0b11110000 + r, sectionCounter + 1);
            symbolTable->insertCode(currentSection, 0b00000011, sectionCounter + 2);
            if (!regex_match(operand, regex("[0-9]+( ?)+")) && (!regex_match(operand, regex("0x[0|1|2|3|4|5|6|7|8|9|A|B|C|D|E|F]+( ?)+")))) {
                if (!symbolTable->containsSymbol(operand)) {
                    symbolTable->addRow(operand, 0, 0, false, -1);
                }
                symbolTable->addUsageListSlot(operand, currentSection, sectionCounter + 3, 1);
                symbolTable->insertCode(currentSection, 0b11111111, sectionCounter + 3);
                symbolTable->insertCode(currentSection, 0b11111111, sectionCounter + 4);
            }
            else {
                int op;
                if (!regex_match(operand, regex("[0-9]+( ?)+"))) {
                    operand = operand.substr(2);
                    op = hexToInt(operand);
                }
                else op = stoi(operand);
                symbolTable->insertCode(currentSection, (op) & 0b11111111, sectionCounter + 3);
                symbolTable->insertCode(currentSection, (op) >> 8, sectionCounter + 4);
            }
            sectionCounter += 5;
        }
        else {
            symbolTable->insertCode(currentSection, 0b11111111, sectionCounter + 1);
            symbolTable->insertCode(currentSection, 0b00000100, sectionCounter + 2);
            if (!regex_match(operand, regex("[0-9]+( ?)+")) && (!regex_match(operand, regex("0x[0|1|2|3|4|5|6|7|8|9|A|B|C|D|E|F]+( ?)+")))) { //simbol
                if (!symbolTable->containsSymbol(operand)) {
                    symbolTable->addRow(operand, 0, 0, false, -1);
                }
                symbolTable->addUsageListSlot(operand, currentSection, sectionCounter + 3, 1);
                symbolTable->insertCode(currentSection, 0b11111111, sectionCounter + 3);
                symbolTable->insertCode(currentSection, 0b11111111, sectionCounter + 4);
            }
            else {
                int op;
                if (!regex_match(operand, regex("[0-9]+( ?)+"))) {
                    operand = operand.substr(2);
                    op = hexToInt(operand);
                }
                else op = stoi(operand);
                symbolTable->insertCode(currentSection, (op) & 0b11111111, sectionCounter + 3);
                symbolTable->insertCode(currentSection, (op) >> 8, sectionCounter + 4);
            }
            sectionCounter += 5;
        }
    }
}

void Asembler::dataInstrctions(string line, int InstrDesc, int ins)
{
    int pos = line.find(",");
    string firsthalf = line.substr(0, pos);
    string secondhalf = line.substr(pos + 1);
    int reg = Checks::getOneRegFromLine(3, firsthalf);
    string operand = Checks::getOperandFromLine(0, secondhalf);
    symbolTable->insertCode(currentSection, InstrDesc, sectionCounter);
    int pos1 = secondhalf.find("$");
    int pos2 = secondhalf.find("%");
    if (pos2 != -1 && (pos1 == -1 || pos2 < pos1)) { 
        operand = operand.substr(1);
        symbolTable->insertCode(currentSection, (reg << 4) + 0b0111, sectionCounter + 1);
        if(ins == 0) symbolTable->insertCode(currentSection, 0b00000011, sectionCounter + 2);
        if(ins == 1) symbolTable->insertCode(currentSection, 0b00010011, sectionCounter + 2);
        if(ins == 2) symbolTable->insertCode(currentSection, 0b01000011, sectionCounter + 2);
        if ((!regex_match(operand, regex("[0-9]+( ?)+"))) && (!regex_match(operand, regex("0x[0|1|2|3|4|5|6|7|8|9|A|B|C|D|E|F]+( ?)+")))) { //simbol
            if (!symbolTable->containsSymbol(operand)) {
                symbolTable->addRow(operand, 0, 0, false, -1);
            }
            symbolTable->addUsageListSlot(operand, currentSection, sectionCounter + 3, 2);
            symbolTable->insertCode(currentSection, 0b11111111, sectionCounter + 3);
            symbolTable->insertCode(currentSection, 0b11111111, sectionCounter + 4);
        }
        else {
            int op;
            if (!regex_match(operand, regex("[0-9]+( ?)+"))) {
                operand = operand.substr(2);
                op = hexToInt(operand);
            }
            else op = stoi(operand);
            symbolTable->insertCode(currentSection, (op) & 0b11111111, sectionCounter);
            symbolTable->insertCode(currentSection, (op) >> 8, sectionCounter);
        }
        sectionCounter += 5;
    }
    if (pos1 != -1 && (pos2 == -1 || pos1 < pos2)) {
        operand = operand.substr(1);
        symbolTable->insertCode(currentSection, (reg << 4) + 0b1111, sectionCounter + 1);
        if (ins == 0) symbolTable->insertCode(currentSection, 0b00000000, sectionCounter + 2);
        if (ins == 1) symbolTable->insertCode(currentSection, 0b00010000, sectionCounter + 2);
        if (ins == 2) symbolTable->insertCode(currentSection, 0b01000000, sectionCounter + 2);

        if ((!regex_match(operand, regex("[0-9]+( ?)+"))) && (!regex_match(operand, regex("0x[0|1|2|3|4|5|6|7|8|9|A|B|C|D|E|F]+( ?)+")))) { //simbol
            if (!symbolTable->containsSymbol(operand)) {
                symbolTable->addRow(operand, 0, 0, false, -1);
            }
            symbolTable->addUsageListSlot(operand, currentSection, sectionCounter + 3, 1);
            symbolTable->insertCode(currentSection, 0b11111111, sectionCounter + 3);
            symbolTable->insertCode(currentSection, 0b11111111, sectionCounter + 4);
        }
        else {
            int op;
            if (!regex_match(operand, regex("[0-9]+( ?)+"))) {
                operand = operand.substr(2);
                op = hexToInt(operand);
            }
            else op = stoi(operand);
            symbolTable->insertCode(currentSection, (op) & 0b11111111, sectionCounter + 3);
            symbolTable->insertCode(currentSection, (op) >> 8, sectionCounter + 4);
        }
        sectionCounter += 5;
    }
    if (pos1 == -1 && pos2 == -1) {
        if (regex_match(operand, regex("((r[0-7])|psw)"))) {
            int r = Checks::getOneRegFromLine(0, operand);
            symbolTable->insertCode(currentSection, (reg << 4) + r, sectionCounter + 1);
            if (ins == 0) symbolTable->insertCode(currentSection, 0b00000001, sectionCounter + 2);
            if (ins == 1) symbolTable->insertCode(currentSection, 0b00010001, sectionCounter + 2);
            if (ins == 2) symbolTable->insertCode(currentSection, 0b01000001, sectionCounter + 2);
            sectionCounter += 3;
        }
        else if (regex_match(operand, regex("(\\[( ?)+((r[0-7])|psw)( ?)+\\])"))) {
            int r = Checks::getOneRegFromLine(1, operand);
            symbolTable->insertCode(currentSection, (reg << 4) + r, sectionCounter + 1);
            if (ins == 0) symbolTable->insertCode(currentSection, 0b00000010, sectionCounter + 2);
            if (ins == 1) symbolTable->insertCode(currentSection, 0b00010010, sectionCounter + 2);
            if (ins == 2) symbolTable->insertCode(currentSection, 0b01000010, sectionCounter + 2);
            sectionCounter += 3;
        }
        else if (regex_match(operand, regex("(\\[( ?)+((r[0-7])|psw)( ?)+\\+( ?)+.+( ?)+\\])"))) {
            int r = Checks::getOneRegFromLine(1, operand);
            int pos = operand.find("+");
            operand = operand.substr(pos + 1);
            operand = Checks::getOperandFromOperand(operand);
            symbolTable->insertCode(currentSection, (reg << 4) + r, sectionCounter + 1);
            if (ins == 0) symbolTable->insertCode(currentSection, 0b00000011, sectionCounter + 2);
            if (ins == 1) symbolTable->insertCode(currentSection, 0b00010011, sectionCounter + 2);
            if (ins == 2) symbolTable->insertCode(currentSection, 0b01000011, sectionCounter + 2);
            if ((!regex_match(operand, regex("[0-9]+( ?)+"))) && (!regex_match(operand, regex("0x[0|1|2|3|4|5|6|7|8|9|A|B|C|D|E|F]+( ?)+")))) {
                if (!symbolTable->containsSymbol(operand)) {
                    symbolTable->addRow(operand, 0, 0, false, -1);
                }
                symbolTable->addUsageListSlot(operand, currentSection, sectionCounter + 3, 1);
                symbolTable->insertCode(currentSection, 0b11111111, sectionCounter + 3);
                symbolTable->insertCode(currentSection, 0b11111111, sectionCounter + 4);
            }
            else {
                int op;
                if (!regex_match(operand, regex("[0-9]+( ?)+"))) {
                    operand = operand.substr(2);
                    op = hexToInt(operand);
                }
                else op = stoi(operand);
                symbolTable->insertCode(currentSection, (op) & 0b11111111, sectionCounter + 3);
                symbolTable->insertCode(currentSection, (op) >> 8, sectionCounter + 4);
            }
            sectionCounter += 5;
        }
        else {
            symbolTable->insertCode(currentSection, (reg << 4) + 0b1111, sectionCounter + 1);
            if (ins == 0) symbolTable->insertCode(currentSection, 0b00000100, sectionCounter + 2);
            if (ins == 1) symbolTable->insertCode(currentSection, 0b00010100, sectionCounter + 2);
            if (ins == 2) symbolTable->insertCode(currentSection, 0b01000100, sectionCounter + 2);
            if ((!regex_match(operand, regex("[0-9]+( ?)+"))) && (!regex_match(operand, regex("0x[0|1|2|3|4|5|6|7|8|9|A|B|C|D|E|F]+( ?)+")))) { //simbol
                if (!symbolTable->containsSymbol(operand)) {
                    symbolTable->addRow(operand, 0, 0, false, -1);
                }
                symbolTable->addUsageListSlot(operand, currentSection, sectionCounter + 3, 1);
                symbolTable->insertCode(currentSection, 0b11111111, sectionCounter + 3);
                symbolTable->insertCode(currentSection, 0b11111111, sectionCounter + 4);
            }
            else {
                int op;
                if (!regex_match(operand, regex("[0-9]+( ?)+"))) {
                    operand = operand.substr(2);
                    op = hexToInt(operand);
                }
                else op = stoi(operand);
                symbolTable->insertCode(currentSection, (op) & 0b11111111, sectionCounter + 3);
                symbolTable->insertCode(currentSection, (op) >> 8, sectionCounter + 4);
            }
            sectionCounter += 5;
        }
    }
}

string Asembler::cutOffStartBlanks(string line)
{
    while ((line.at(0) == ' ') || (line.at(0) == '\t')) {
        line = line.substr(1);
    }
    return line;
}

int Asembler::hexToInt(string hex)
{
    int ret = 0;
    for (int i = 0; i < hex.size(); i++) {
        int it;
        if (hex.at(i) == 'A') {
            it = 10;
        }
        else if (hex.at(i) == 'B') {
            it = 11;
        }
        else if (hex.at(i) == 'C') {
            it = 12;
        }
        else if (hex.at(i) == 'D') {
            it = 13;
        }
        else if (hex.at(i) == 'E') {
            it = 14;
        }
        else if (hex.at(i) == 'F') {
            it = 15;
        }
        else {
            string s = hex.at(i) + "";
            it = stoi(s);
        }
        ret = (ret << 4) | it;
    }
    return ret;
}

