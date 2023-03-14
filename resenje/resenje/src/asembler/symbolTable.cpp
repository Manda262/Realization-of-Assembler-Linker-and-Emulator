#include "symbolTable.h"

int SymbolTable::numberCounter = 0;

SymbolTable::SymbolTable()
{
  rows.push_back(RowOfSymbolTable("UND", 0, 0, false, SymbolTable::numberCounter++ ,0));
}

void SymbolTable::addRow(string SymNm, int SecNum, int val, bool isG, int sz)
{
	rows.push_back(RowOfSymbolTable(SymNm, SecNum, val, isG, SymbolTable::numberCounter++, sz));
}

bool SymbolTable::containsSymbol(string sym)
{
	for (RowOfSymbolTable row : rows) {
		if (row.SymbolName.compare(sym) == 0) {
			return true;
		}
	}
	return false;
}

void SymbolTable::changeSize(string sym, int sz)
{
	for (int i = 0; i < rows.size(); i++) {
		if (rows.at(i).SymbolName.compare(sym) == 0) {
			rows.at(i).size = sz;
		}
	}
}

void SymbolTable::changeValue(string sym, int val, string sec)
{
	for (int i = 0; i < rows.size(); i++) {
		if (rows.at(i).SymbolName.compare(sym) == 0) {
			rows.at(i).value = val;
			rows.at(i).SectionNumber = this->getRowWithName(sec).number;
		}
	}
}

void SymbolTable::addUsageListSlot(string sym,string sec, int addr, int t)
{
	if (containsSymbol(sym)) {
		for (int i = 0; i < rows.size(); i++) {
			if (rows.at(i).SymbolName.compare(sym) == 0) {
				rows.at(i).usageList.push_back(UsageListSlot(sec,addr, t));
			}
		}
	}
}

void SymbolTable::insertCode(string sec, char code, int secCnt)
{
	if (containsSymbol(sec)) {
		for (int i = 0; i < rows.size(); i++) {
			if (rows.at(i).SymbolName.compare(sec) == 0) {
				rows.at(i).code.push_back((code));
			}
		}
	}
}

void SymbolTable::replaceCode(string sec, int addr, int code)
{
	if (containsSymbol(sec)) {
		for (int i = 0; i < rows.size(); i++) {
			if (rows.at(i).SymbolName.compare(sec) == 0) {
				rows.at(i).code.at(addr) = code & 0b11111111;
				rows.at(i).code.at(addr + 1) = (code >> 8);
			}
		}
	}
}

void SymbolTable::resolveAndGenerate(vector<string> externSymbols)
{
	for (int i = 0; i < rows.size(); i++) {
		RowOfSymbolTable row = rows.at(i);
		for (int j = 0; j < row.usageList.size(); j++) { // provera za nedefinisan simbol
			if (row.SectionNumber == 0 && (!isExtern(row.SymbolName, externSymbols))) {
				throw invalid_argument("Nedefinisan simbol");
			}
			UsageListSlot slot = row.usageList.at(j);
			if (row.isGlobal) {
				if (slot.type == 1) replaceCode(slot.section, slot.address, 0);
				else replaceCode(slot.section, slot.address, -2);
			}
			else {
				if (slot.type == 1) replaceCode(slot.section, slot.address, row.value);
				else replaceCode(slot.section, slot.address, row.value - 2);
			}
		}
	}
}

RowOfSymbolTable SymbolTable::getRowAt(int i)
{
	return rows.at(i);
}

RowOfSymbolTable SymbolTable::getRowWithName(string sym)
{
	for (int i = 0; i < rows.size(); i++) {
		if (rows.at(i).SymbolName == sym) {
			return rows.at(i);
		}
	}
	throw invalid_argument("Greska u sistemu");
}

int SymbolTable::getSizeAt(int i)
{
	return rows.at(i).size;
}

string SymbolTable::getSymNameAt(int i)
{
	return rows.at(i).SymbolName;
}

void SymbolTable::fillCodeWithZeros(int ind, int addr)
{
	for (int i = rows.at(ind).code.size(); i < addr; i++) {
		rows.at(ind).code.push_back(0b11111111);
	}
}

void SymbolTable::writeToFile(string output)
{
	fstream outputFile;
	outputFile.open(output, ios::app);
	if (outputFile.is_open() == false) throw invalid_argument("Argumenti nisu validni!");

	for (int i = 0; i < rows.size(); i++) {
		outputFile << rows.at(i).SymbolName << ":" << rows.at(i).SectionNumber << ":" << rows.at(i).value << ":"
			<< rows.at(i).isGlobal << ":" << rows.at(i).number << ":" << rows.at(i).size << "\n";
	}

	outputFile.close();

}

void SymbolTable::writeCodeToFileAt(string output, string section)
{

	fstream outputFile;
	outputFile.open(output, fstream::in | fstream::out | fstream::app); // otvori ako postoji napravi ako ne postoji
	if (outputFile.is_open() == false) throw invalid_argument("Argumenti nisu validni!");

	int num = -1;
	for (int i = 0; i < rows.size();i++) {
		if (rows.at(i).SymbolName.compare(section) == 0) { // isti
			num = i;
		}
	}

	if (num == -1) return;
	for (int i = 0; i < rows.at(num).code.size(); i++) {
		if (i != 0) outputFile << ":";
		outputFile << (int)rows.at(num).code.at(i);
	}

	outputFile << "\n";

	outputFile.close();
}

bool SymbolTable::isExtern(string sym, vector<string> externSymbols)
{
	for (string symbol : externSymbols) {
		if (!symbol.compare(sym)) { // ako je nasao simbol u listi externih
			return true;
		}
	}
	return false;
}

void SymbolTable::changeGlobal(string sym)
{

	for (int i = 0; i < rows.size(); i++) {
		if (rows.at(i).SymbolName.compare(sym) == 0) {
			rows.at(i).isGlobal = true;
		}
	}
}




