#ifndef _linker_h_
#define _linker_h_

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <regex>
#include <bitset>
#include <iostream>

using namespace std;

struct GlobalSymbolSlot{
	string symbol;
	int value;
	GlobalSymbolSlot(string s, int v) {
		symbol = s;
		value = v;
	}
};

struct SectionPosition {
	string section;
	int position;
	SectionPosition(string s, int p) {
		section = s;
		position = p;
	}
};

struct SectionNames {
	int size;
	string name;
	int startingPosition;
	vector<char> sectionCode;
	SectionNames(int s, string n) {
		size = s;
		name = n;
		startingPosition = -1;
	}
};

struct FileSectionNames {
	string filename;
	vector<SectionNames> sectionNames;
	FileSectionNames(string f) {
		filename = f;
	}
};

class Linker {
public:
	Linker(char* argv[], int argc, bool hex);
	void start();
	static int checkArgsLength(int argc, char* argv[]);
private:

	vector<int> code;

	vector<string> inputs;
	string output;
	vector<FileSectionNames> fileSectionNames; // pomocni vektor za skupljanje svih sekcija
	vector<SectionPosition> sectionPositions; // vektor sa pozicijama svih sekcija svih fajlova
	int currSecPos;
	vector<GlobalSymbolSlot> globalSymbols;

	void fillFileSectionNames();
	void fillSectionPositions();
	void makeTableOfGlobalSymbols();
	void makeCode();
	void makeCodeForSections();
	void dealWithRelocationTables();
	void makeOutFile();

	bool checkIfAlreadyExists(string section, vector<SectionPosition> sectionPositions);
	void findIfSameSectionExistSomewhere(int i, int j);
	int findSection(string section, FileSectionNames f); // vraca pocetnu adresu sekcije
	string findSectionFromSecNum(string secN, int i);
	bool existsAlreadyInGlobalSymbols(string symbol);
	int findSecInFSN(FileSectionNames f, string section);
	void resolveForSTRef(string stref, int offset, int type, int i, string sectionUse);
	int findValueOfSymbolInGS(string symbol);
	string decToBinary(int n);

	int checkSign(int data);
};

#endif

