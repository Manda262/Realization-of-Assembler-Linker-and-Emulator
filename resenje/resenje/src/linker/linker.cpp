#include "linker.h"

Linker::Linker(char* argv[], int argc, bool hex)
{
	currSecPos = 0;
	regex inputreg("([^ ^.]+\\.o)");
	if (hex) {
		output = argv[3];
		for (int i = 4; i < argc; i++) {
			if (regex_match(argv[i], inputreg)) {
				inputs.push_back(argv[i]);
			}
		}
	}
	else {
		for (int i = 3; i < argc; i++) {
			if (regex_match(argv[i], inputreg)) inputs.push_back(argv[i]);
		}
	}
}

void Linker::start()
{
	fillFileSectionNames();
	fillSectionPositions();
	makeTableOfGlobalSymbols();
	makeCodeForSections();
	makeCode();
	dealWithRelocationTables();
	makeOutFile();
}

void Linker::fillFileSectionNames()
{
	for (int i = 0; i < inputs.size(); i++) {
		fstream inputFile;
		inputFile.open(inputs.at(i));
		if (inputFile.is_open() == false) throw invalid_argument("Argumenti nisu validni!");
		string line;
		fileSectionNames.push_back(FileSectionNames(inputs.at(i)));
		while (getline(inputFile, line)) {
			if (line.compare("Relokacione tabele") == 0) { // isti
				break;
			}
			if (line.compare("SymbolTable") == 0) continue;
			if(regex_match(line, regex("(( ?)+)"))) break;
			string substr = line;
			for (int j = 0; j < 5; j++) {
				substr = substr.substr(substr.find(":") + 1);
			}
			int size = stoi(substr);
			if (size != -1) {
				fileSectionNames.at(fileSectionNames.size() - 1).
					sectionNames.push_back(SectionNames(size, line.substr(0, line.find(":"))));
			}
		}
		inputFile.close();
	}
}

void Linker::fillSectionPositions()
{
	for (int i = 0; i < fileSectionNames.size(); i++) {
		for (int j = 0; j < fileSectionNames.at(i).sectionNames.size(); j++) {
			string sec = fileSectionNames.at(i).sectionNames.at(j).name;
			if(checkIfAlreadyExists(sec, sectionPositions)) continue;
			int size = fileSectionNames.at(i).sectionNames.at(j).size;
			sectionPositions.push_back(SectionPosition(sec, currSecPos));
			fileSectionNames.at(i).sectionNames.at(j).startingPosition = currSecPos;
			currSecPos += size;
			findIfSameSectionExistSomewhere(i,j);
		}
	}
}

void Linker::makeTableOfGlobalSymbols()
{
	for (int i = 0; i < inputs.size(); i++) {
		fstream inputFile;
		inputFile.open(inputs.at(i));
		if (inputFile.is_open() == false) throw invalid_argument("Argumenti nisu validni!");
		string line;
		while (getline(inputFile, line)) {
			if (line.compare("Relokacione tabele") == 0) {
				break;
			}
			if (line.compare("SymbolTable") == 0) continue;
			if (regex_match(line, regex("(( ?)+)"))) break;
			string size = line;
			string secNum = line;
			string symbol = line.substr(0, line.find(":"));
			secNum = secNum.substr(secNum.find(":") + 1);
			secNum = secNum.substr(0, secNum.find(":"));
			int sectionNumber = stoi(secNum);
			string section = findSectionFromSecNum(secNum, i);
			for (int j = 0; j < 5; j++) {
				size = size.substr(size.find(":") + 1);
			}
			int numSize = stoi(size);

			string val = line;
			val = val.substr(val.find(":") + 1);
			val = val.substr(val.find(":") + 1);
			val = val.substr(0, val.find(":"));
			int value = stoi(val);

			if (sectionNumber != 0 && numSize == -1) { // nije sekcija i nije eksteran = globalan
				if (existsAlreadyInGlobalSymbols(symbol)) throw invalid_argument("Visestruko imenovanje simbola");
				int addr = findSection(section, fileSectionNames.at(i));
				globalSymbols.push_back(GlobalSymbolSlot(symbol, addr + value));
			}
		}
	}
}

void Linker::makeCode()
{
	for (int i = 0; i < fileSectionNames.size(); i++) {
		for (int j = 0; j < fileSectionNames.at(i).sectionNames.size(); j++) {
			for (int k = 0; k < fileSectionNames.at(i).sectionNames.at(j).sectionCode.size(); k++) {
				int addr = fileSectionNames.at(i).sectionNames.at(j).startingPosition;
				code.at(addr + k) = fileSectionNames.at(i).sectionNames.at(j).sectionCode.at(k);
			}
		}
	}
}

void Linker::makeCodeForSections()
{
	regex rel("(Relokaciona tabela za sekciju: .+)");
	for (int i = 0; i < inputs.size(); i++) {
		fstream inputFile;
		inputFile.open(inputs.at(i));
		if (inputFile.is_open() == false) throw invalid_argument("Argumenti nisu validni!");
		string line;
		string section;
		while (getline(inputFile, line)) {
			if (regex_match(line, rel)) {
				section = line.substr(31); // ostavlja samo ime sekcije
			}
			if (line.compare("kod sekcije:") == 0) {
				if(!getline(inputFile, line)) break;
				string substr = line;
				int j = findSecInFSN(fileSectionNames.at(i), section);
				while (1) {
					int pos = substr.find(":");
					if (pos == -1) break;
					int c = stoi(substr.substr(0, pos));
					substr = substr.substr(pos+1);
					fileSectionNames.at(i).sectionNames.at(j).sectionCode.push_back(c);
					code.push_back(0);
				}
				int c = stoi(substr);
				fileSectionNames.at(i).sectionNames.at(j).sectionCode.push_back(c);
				code.push_back(0);
			}
		}
	}
}

void Linker::dealWithRelocationTables()
{
	regex rel("(Relokaciona tabela za sekciju: .+)");
	for (int i = 0; i < inputs.size(); i++) {
		fstream inputFile;
		inputFile.open(inputs.at(i));
		if (inputFile.is_open() == false) throw invalid_argument("Argumenti nisu validni!");
		string line;
		string section;
		int stop = 0;
		while (getline(inputFile, line)) {
			if (regex_match(line, rel)) {
				section = line.substr(31); // ostavlja samo ime sekcije
				while (getline(inputFile, line)) {
					if (line.compare("kod sekcije:") == 0) break;
					string off = line.substr(line.find(":") + 1);
					string stref = off.substr(off.find(":") + 1);
					off = off.substr(0, off.find(":"));
					int offset = stoi(off);
					string t = line.substr(0, line.find(":"));
					int type = stoi(t);
					resolveForSTRef(stref, offset, type, i, section);
				}
			}
		}
		inputFile.close();
	}
}

void Linker::makeOutFile()
{
	fstream outputFile;
	outputFile.open(output, fstream::in | fstream::out | fstream::app);
	if (outputFile.is_open() == false) throw invalid_argument("Argumenti nisu validni!");
	int addr = 0;
	for (int i = 0; i < code.size();) {
		outputFile << decToBinary(addr) << ": ";
		for (int j = 0; j < 8 && i < code.size(); i++, j++) {
			addr++;
			outputFile << code.at(i) << " ";
		}
		outputFile << "\n";
	}
	outputFile.close();
}

bool Linker::checkIfAlreadyExists(string section, vector<SectionPosition> sectionPositions)
{
	for (int i = 0; i < sectionPositions.size(); i++) {
		if (section.compare(sectionPositions.at(i).section) == 0) { // isti
			return true;
		}
	}
	return false;
}

int Linker::checkArgsLength(int argc, char* argv[])
{
	if (argc < 4) throw invalid_argument("Argumenti nisu validni");
	regex optionreg1("-o");
	regex optionreg2("-hex");
	regex inputreg("([^ ^.]+\\.o)");
	regex outputreg("([^ ^.]+\\.hex)");
	if (regex_match(argv[1], optionreg2)) {
		if (regex_match(argv[2], optionreg1) && regex_match(argv[3], outputreg)) {
			return argc;
		}
		else return -2;
	}
	else {
		return -1;
	}
}

void Linker::findIfSameSectionExistSomewhere(int i, int j)
{
	string sec = fileSectionNames.at(i).sectionNames.at(j).name;

	for (int k = 0; k < fileSectionNames.size(); k++) {
		if (k == i) continue;
		for (int l = 0; l < fileSectionNames.at(k).sectionNames.size(); l++) {
			string trySec = fileSectionNames.at(k).sectionNames.at(l).name;
			if (trySec.compare(sec) == 0) {
				int size = fileSectionNames.at(k).sectionNames.at(l).size;
				fileSectionNames.at(k).sectionNames.at(l).startingPosition = currSecPos;
				currSecPos += size;
			}
		}
	}
}

int Linker::findSection(string section, FileSectionNames f)
{
	for (int i = 0; i < f.sectionNames.size(); i++) {
		if (section.compare(f.sectionNames.at(i).name) == 0) { // isti
			return f.sectionNames.at(i).startingPosition;
		}
	}
	throw invalid_argument("Greska u sistemu");
}

string Linker::findSectionFromSecNum(string secN, int i)
{
	fstream inputFile;
	inputFile.open(inputs.at(i));
	if (inputFile.is_open() == false) throw invalid_argument("Argumenti nisu validni!");
	string line;
	while (getline(inputFile, line)) {
		if (line.compare("SymbolTable") == 0) continue;
		string secNum = line;
		secNum = secNum.substr(secNum.find(":") + 1);
		secNum = secNum.substr(0, secNum.find(":"));

		string size = line;
		for (int j = 0; j < 5; j++) {
			size = size.substr(size.find(":") + 1);
		}
		int numSize = stoi(size);
		if (numSize != -1 && (secN.compare(secNum) == 0)) { // sekcija sa datim brojem sekcije
			return line.substr(0, line.find(":"));
		}
	}
	throw invalid_argument("Greska u sistemu");
}

bool Linker::existsAlreadyInGlobalSymbols(string symbol)
{
	for (int i = 0; i < globalSymbols.size(); i++) {
		if (globalSymbols.at(i).symbol.compare(symbol) == 0) return true;
	}
	return false;
}

int Linker::findSecInFSN(FileSectionNames f, string section)
{
	for (int i = 0; i < f.sectionNames.size(); i++) {
		if (section.compare(f.sectionNames.at(i).name) == 0) return i;
	}
	throw invalid_argument("Greska u sistemu");
}

void Linker::resolveForSTRef(string stref, int offset, int type, int i, string sectionUse)
{
	fstream inputFile;
	inputFile.open(inputs.at(i));
	if (inputFile.is_open() == false) throw invalid_argument("Argumenti nisu validni!");
	string line;
	while (getline(inputFile, line)) {
		if (line.compare("Relokacione tabele") == 0) {
			break;
		}
		if (line.compare("SymbolTable") == 0) continue;
		if (regex_match(line, regex("(( ?)+)"))) break;
		string number = line;
		for (int j = 0; j < 4; j++) {
			number = number.substr(number.find(":") + 1);
		}
		string sz = number.substr(number.find(":") + 1);
		int size = stoi(sz);
		number = number.substr(0, number.find(":"));
		if (number.compare(stref) == 0) {
			if (size == -1) { // simbol
				string symbol = line.substr(0, line.find(":"));
				string secNum = line.substr(line.find(":") + 1);
				secNum = secNum.substr(0, secNum.find(":"));
				string section = findSectionFromSecNum(secNum, i);
				int startAddr = findSection(sectionUse, fileSectionNames.at(i));
				int value = findValueOfSymbolInGS(symbol);
				int old = ((code.at(offset + startAddr + 1) << 8)) + (code.at(offset + startAddr) & 0xFF);
				if (value == -2) throw invalid_argument("Postoje nerazreseni simboli");
				if (type == 1) {
					code.at(offset + startAddr) = checkSign(value & 0b11111111);
					code.at(startAddr + offset + 1) = checkSign(value >> 8);
				}
				else {
					code.at(offset + startAddr) = checkSign((value + old - (startAddr + offset)) & 0b11111111);
					code.at(startAddr + offset + 1) = checkSign((((value + old - (startAddr + offset)) >> 8) & 0b11111111));
				}
			}
			else { // section (lokalni simbol)
				string section = line.substr(0, line.find(":")); // sekcija u kojoj je definisan simbol
				int startAddr = findSection(sectionUse, fileSectionNames.at(i));
				int adding = findSection(section, fileSectionNames.at(i));
				if (type == 1) {
					int newval = (code.at(offset + startAddr + 1) << 8) + code.at(offset + startAddr) + adding;
					code.at(offset + startAddr) = checkSign(newval & 0b11111111);
					code.at(offset + startAddr + 1) = checkSign(newval >> 8);
				}
				else {
					int newval = ((code.at(offset + startAddr + 1) << 8) + code.at(offset + startAddr)) + adding - (startAddr + offset);
					code.at(offset + startAddr) = checkSign(newval & 0b11111111);
					code.at(offset + startAddr + 1) = checkSign(newval >> 8);
				}
			}
		}
	}
}

int Linker::findValueOfSymbolInGS(string symbol)
{
	for (int i = 0; i < globalSymbols.size(); i++) {
		if (symbol.compare(globalSymbols.at(i).symbol) == 0) return globalSymbols.at(i).value;
	}
	return -2;
}

string Linker::decToBinary(int n)
{

	std::stringstream ss;
	ss << std::hex << n;
	std::string result = ss.str();

	if (result.size() == 0) result.insert(0, "0000");
	else if (result.size() == 1) result.insert(0, "000");
	else if (result.size() == 2) result.insert(0, "00");
	else if (result.size() == 3) result.insert(0, "0");

	return result;
}

int Linker::checkSign(int data) 
{
	if ((data & 0x80) == 0) {
		return data & 0xFF;
	}
	else {
		return (((-1)*((0xFF - data) + 1)));
	}
}
