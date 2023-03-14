#include "emulator.h"

int Emulator::MIN_ADRR = 40000;

Emulator::Emulator(string in)
{
	input = in;
	end = false;
	r0 = 0;
	r1 = 0;
	r2 = 0;
	r3 = 0;
	r4 = 0;
	r5 = 0;
}

void Emulator::start()
{
	fillMemory();
	emulate();
	writeOut();
}

void Emulator::fillMemory()
{
	fstream inputFile;
	inputFile.open(input);
	if (inputFile.is_open() == false) throw invalid_argument("Argumenti nisu validni!");
	string line;
	int iter = 0;
	while (getline(inputFile, line)) {
		string substr = line.substr(6);
		string datastr;
		for (int i = 0; i < 8 && substr != ""; i++) {
			if (substr.find(" ") == -1) datastr = substr;
			else datastr = substr.substr(0, substr.find(" "));
			int data = stoi(datastr);
			memory[iter] = data;
			iter++;
			if (substr.find(" ") == -1) break;
			else substr = substr.substr(substr.find(" ") + 1);
		}
	}
	sizeCode = iter;
}

void Emulator::emulate()
{
	pc = (memory[1] & 0xFF) << 8 | (memory[0] & 0xFF);
	psw = 0;
	sp = 0xFFFF;
	while (!end) {
		Instruction instruction = instructionFetch();
		executeInstruction(instruction);
	}
}

void Emulator::writeOut()
{
	cout << "------------------------------------------------\n";
	cout << "Emulated processor executed halt instruction\n";
	cout << "Emulated processor state: psw=" << intToBin(psw, 16);
	cout << "\nr0=" << intToHex(r0) << "   r1=" << intToHex(r1);
	cout << "   r2=" << intToHex(r2) << "   r3=" << intToHex(r3);
	cout << "\nr4=" << intToHex(r4) << "   r5=" << intToHex(r5);
	cout << "   r6=" << intToHex(sp) << "   r7=" << intToHex(pc);
	cout << "\n" << "\n";
}

Instruction Emulator::instructionFetch()
{
	int instrDescr = memory[pc] & 0xFF;
	pc++;
	if (instrDescr == 0b00000000) {
		return Instruction(1, instrDescr);
	}
	else if (instrDescr == 0b00100000) {
		return Instruction(2, instrDescr);
	}
	else if (instrDescr == 0b01000000) {
		return Instruction(3, instrDescr);
	}
	else {
		int regsDescr = memory[pc] & 0xFF;
		pc++;
		if (instrDescr == 0b00010000) {
			return Instruction(4, instrDescr, regsDescr);
		}
		else if (instrDescr == 0b01100000) {
			return Instruction(5, instrDescr, regsDescr);
		}
		else if (instrDescr == 0b01110000) {
			return Instruction(6, instrDescr, regsDescr);
		}
		else if (instrDescr == 0b01110001) {
			return Instruction(7, instrDescr, regsDescr);
		}
		else if (instrDescr == 0b01110010) {
			return Instruction(8, instrDescr, regsDescr);
		}
		else if (instrDescr == 0b01110011) {
			return Instruction(9, instrDescr, regsDescr);
		}
		else if (instrDescr == 0b01110100) {
			return Instruction(10, instrDescr, regsDescr);
		}
		else if (instrDescr == 0b10000000) {
			return Instruction(11, instrDescr, regsDescr);
		}
		else if (instrDescr == 0b10000001) {
			return Instruction(12, instrDescr, regsDescr);
		}
		else if (instrDescr == 0b10000010) {
			return Instruction(13, instrDescr, regsDescr);
		}
		else if (instrDescr == 0b10000011) {
			return Instruction(14, instrDescr, regsDescr);
		}
		else if (instrDescr == 0b10000100) {
			return Instruction(15, instrDescr, regsDescr);
		}
		else if (instrDescr == 0b10010000) {
			return Instruction(16, instrDescr, regsDescr);
		}
		else if (instrDescr == 0b10010001) {
			return Instruction(17, instrDescr, regsDescr);
		}
		else {
			int addrMode = memory[pc];
			pc++;
			if (((addrMode & 0b1111) == 0b0001) || ((addrMode & 0b1111) == 0b0010)) {
				if (instrDescr == 0b00110000) {
					return Instruction(18, instrDescr, regsDescr, addrMode);
				}
				if (instrDescr == 0b01010000) {
					return Instruction(19, instrDescr, regsDescr, addrMode);
				}
				if (instrDescr == 0b01010001) {
					return Instruction(20, instrDescr, regsDescr, addrMode);
				}
				if (instrDescr == 0b01010010) {
					return Instruction(21, instrDescr, regsDescr, addrMode);
				}
				if (instrDescr == 0b01010011) {
					return Instruction(22, instrDescr, regsDescr, addrMode);
				}
				if (instrDescr == 0b10100000) {
					return Instruction(23, instrDescr, regsDescr, addrMode);
				}
				if (instrDescr == 0b10110000) {
					return Instruction(24, instrDescr, regsDescr, addrMode);
				}
			}
			else {
				int dataLow = memory[pc]; // prvi (levlji) bajtovi
				pc++;
				int dataHigh = memory[pc]; // drugi (desnji) barjtovi
				pc++;
				if (instrDescr == 0b00110000) {
					return Instruction(18, instrDescr, regsDescr, addrMode, dataHigh, dataLow);
				}
				if (instrDescr == 0b01010000) {
					return Instruction(19, instrDescr, regsDescr, addrMode, dataHigh, dataLow);
				}
				if (instrDescr == 0b01010001) {
					return Instruction(20, instrDescr, regsDescr, addrMode, dataHigh, dataLow);
				}
				if (instrDescr == 0b01010010) {
					return Instruction(21, instrDescr, regsDescr, addrMode, dataHigh, dataLow);
				}
				if (instrDescr == 0b01010011) {
					return Instruction(22, instrDescr, regsDescr, addrMode, dataHigh, dataLow);
				}
				if (instrDescr == 0b10100000) {
					return Instruction(23, instrDescr, regsDescr, addrMode, dataHigh, dataLow);
				}
				if (instrDescr == 0b10110000) {
					return Instruction(24, instrDescr, regsDescr, addrMode, dataHigh, dataLow);
				}
			}
		}
	}
}

void Emulator::executeInstruction(Instruction instruction)
{
	switch (instruction.instruction) {
	case 1:
		executeHalt(instruction);
		break;
	case 2:
		executeIret(instruction);
		break;
	case 3:
		executeRet(instruction);
		break;
	case 4:
		executeInt(instruction, -1);
		break;
	case 5:
		executeXchg(instruction);
		break;
	case 6:
		executeAdd(instruction);
		break;
	case 7:
		executeSub(instruction);
		break;
	case 8:
		executeMul(instruction);
		break;
	case 9:
		executeDiv(instruction);
		break;
	case 10:
		executeCmp(instruction);
		break;
	case 11:
		executeNot(instruction);
		break;
	case 12:
		executeAnd(instruction);
		break;
	case 13:
		executeOr(instruction);
		break;
	case 14:
		executeXor(instruction);
		break;
	case 15:
		executeTest(instruction);
		break;
	case 16:
		executeShl(instruction);
		break;
	case 17:
		executeShr(instruction);
		break;
	case 18:
		executeCall(instruction);
		break;
	case 19:
		executeJmp(instruction);
		break;
	case 20:
		executeJeq(instruction);
		break;
	case 21:
		executeJne(instruction);
		break;
	case 22:
		executeJgt(instruction);
		break;
	case 23:
		executeLdrPop(instruction);
		break;
	case 24:
		executeStrPush(instruction);
		break;
	}
}

void Emulator::executeHalt(Instruction instruction)
{
	end = true;
}

void Emulator::executeIret(Instruction instruction)
{
	int pswFromStack = pop();
	psw = pswFromStack;
	int pcFromStack = pop();
	pc = pcFromStack;
}

void Emulator::executeRet(Instruction instruction)
{
	int pcFromStack = pop();
	pc = pcFromStack;
}

void Emulator::executeInt(Instruction instruction, int num)
{
	push(pc);
	push(psw);
	psw |= 0x8000; // sto ovo?
	if(num == -1) pc = (memory[(findRegFromRegDescr(instruction.regsDescr >> 4) % 8) * 2] & 0xFF) | ((memory[(findRegFromRegDescr(instruction.regsDescr >> 4) % 8) * 2 + 1] & 0xFF) << 8);
	else pc = (memory[num * 2] & 0xFF) | ((memory[num * 2 + 1] & 0xFF) << 8);
}

void Emulator::executeXchg(Instruction instruction)
{
	int& regD = findRegFromRegDescr(instruction.regsDescr >> 4);
	int tmp = regD;
	int& regS = findRegFromRegDescr(instruction.regsDescr & 0b1111);
	regD = regS;
	regS = tmp;
}

void Emulator::executeAdd(Instruction instruction)
{
	int& regD = findRegFromRegDescr(instruction.regsDescr >> 4);
	int regDOld = regD;
	int& regS = findRegFromRegDescr(instruction.regsDescr & 0b1111);
	regD += regS;
	regD = checkSign(regD);
}

void Emulator::executeSub(Instruction instruction)
{
	int& regD = findRegFromRegDescr(instruction.regsDescr >> 4);
	int& regS = findRegFromRegDescr(instruction.regsDescr & 0b1111);
	int regDOld = regD;
	regD -= regS;
	regD = checkSign(regD);
}

void Emulator::executeMul(Instruction instruction)
{
	int& regD = findRegFromRegDescr(instruction.regsDescr >> 4);
	int& regS = findRegFromRegDescr(instruction.regsDescr & 0b1111);
	regD *= regS;
	regD = checkSign(regD);
}

void Emulator::executeDiv(Instruction instruction)
{
	int& regS = findRegFromRegDescr(instruction.regsDescr & 0b1111);
	if (regS == 0) {
		int temp1 = 0b0001;
		int temp2 = instruction.regsDescr & 0b1111;
		instruction.regsDescr = (temp2 << 4) | temp1;
		executeInt(instruction, 1);
	}
	int& regD = findRegFromRegDescr(instruction.regsDescr >> 4);
	regD /= regS;
	regD = checkSign(regD);
}

void Emulator::executeCmp(Instruction instruction)
{
	int& regD = findRegFromRegDescr(instruction.regsDescr >> 4);
	int& regS = findRegFromRegDescr(instruction.regsDescr & 0b1111);
	int res = regD - regS;
	res = checkSign(res);
	updateZ(res);
	updateN(res);
	updateO(res, regD, regS);
	if(regD < regS) psw |= 0x4;
	else psw &= 0xFFFB;
}

void Emulator::executeNot(Instruction instruction)
{
	int& regD = findRegFromRegDescr(instruction.regsDescr >> 4);
	regD = ~regD;
	regD = checkSign(regD);
}

void Emulator::executeAnd(Instruction instruction)
{
	int& regD = findRegFromRegDescr(instruction.regsDescr >> 4);
	int& regS = findRegFromRegDescr(instruction.regsDescr & 0b1111);
	regD &= regS;
	regD = checkSign(regD);
}

void Emulator::executeOr(Instruction instruction)
{
	int& regD = findRegFromRegDescr(instruction.regsDescr >> 4);
	int& regS = findRegFromRegDescr(instruction.regsDescr & 0b1111);
	regD |= regS;
	regD = checkSign(regD);
}

void Emulator::executeXor(Instruction instruction)
{
	int& regD = findRegFromRegDescr(instruction.regsDescr >> 4);
	int& regS = findRegFromRegDescr(instruction.regsDescr & 0b1111);
	regD ^= regS;
	regD = checkSign(regD);
}

void Emulator::executeTest(Instruction instruction)
{
	int& regD = findRegFromRegDescr(instruction.regsDescr >> 4);
	int& regS = findRegFromRegDescr(instruction.regsDescr & 0b1111);
	int res = regD & regS;
	res = checkSign(res);
	updateZ(res);
	updateN(res);
}

void Emulator::executeShl(Instruction instruction)
{
	int& regD = findRegFromRegDescr(instruction.regsDescr >> 4);
	int& regS = findRegFromRegDescr(instruction.regsDescr & 0b1111);
	if (regS < 0) {
		int temp1 = 0b0001;
		int temp2 = instruction.regsDescr & 0b1111;
		instruction.regsDescr = (temp2 << 4) | temp1;
		executeInt(instruction, 1);
	}
	int regDOld = regD;
	regD <<= regS;
	regD = checkSign(regD);
	updateZ(regD);
	updateN(regD);
	if (regS == 0) {
		psw &= 0xFFFB;
		return;
	}
	if (regS >= 16 && regDOld != 0) {
		psw |= 0x4;
		return;
	}
	int hlp = regDOld >> (sizeof(int) - regS);
	if(hlp != 0) psw |= 0x4;
	else psw &= 0xFFFB;
}

void Emulator::executeShr(Instruction instruction)
{
	int& regD = findRegFromRegDescr(instruction.regsDescr >> 4);
	int& regS = findRegFromRegDescr(instruction.regsDescr & 0b1111);
	if (regS < 0) {
		int temp1 = 0b0001;
		int temp2 = instruction.regsDescr & 0b1111;
		instruction.regsDescr = (temp2 << 4) | temp1;
		executeInt(instruction, 1);
	}
	int regDOld = regD;
	regD >>= regS;
	regD = checkSign(regD);
	updateZ(regD);
	updateN(regD);
	if (regS == 0) {
		psw &= 0xFFFB;
		return;
	}
	if (regS >= 16 && regDOld != 0) {
		psw |= 0x4;
		return;
	}
	int hlp = regDOld << (sizeof(int) - regS);
	if(hlp != 0) psw |= 0x4;
	else psw &= 0xFFFB;
}

void Emulator::executeCall(Instruction instruction)
{
	push(pc);
	pc = getValueFromOperand(instruction);
}

void Emulator::executeJmp(Instruction instruction)
{
	pc = getValueFromOperand(instruction);
}

void Emulator::executeJeq(Instruction instruction)
{
	if(psw & 0b1) pc = getValueFromOperand(instruction);
}

void Emulator::executeJne(Instruction instruction)
{
	if (!(psw & 0b1)) pc = getValueFromOperand(instruction);
}

void Emulator::executeJgt(Instruction instruction)
{
	if(!((psw & 0x8) >> 3) && !((psw & 0b1))) pc = getValueFromOperand(instruction);
}

void Emulator::executeLdrPop(Instruction instruction)
{
	int& regD = findRegFromRegDescr(instruction.regsDescr >> 4);

	if ((instruction.addrMode >> 4) != 0) { //pop
		regD = pop();
	}
	else { //ldr
		regD = getValueFromOperand(instruction);
	}
}

void Emulator::executeStrPush(Instruction instruction)
{
	int regD = findRegFromRegDescr(instruction.regsDescr >> 4) ;
	if ((instruction.addrMode >> 4) != 0) { //push
		push(regD);
	}
	else { //str
		storeInOperand(instruction, regD);
	}
}

int Emulator::pop()
{
	if (sp + 1 > 0xFFFF) throw invalid_argument("StackOverflow");
	int hlp = pc;
	int high = memory[sp + 1];
	int low = memory[sp];
	int ret = (high & 0xFF) << 8 | (low & 0xFF);
	sp += 2;
	pc = hlp;
	return ret;

}
void Emulator::push(int data)
{
	if (sp - 2 < MIN_ADRR) throw invalid_argument("StackUnderflow");
	int high = (data >> 8) & 0xFF;
	int low = (data & 0xFF);
	int hlp = pc;
	--sp;
	memory[sp] = high;
	--sp;
	memory[sp] = low;
	pc = hlp;
}

int Emulator::getValueFromOperand(Instruction instruction)
{
	if ((instruction.addrMode & 0xF) == 0b0000) { // neposredno 5
		return ((instruction.dataHigh & 0xFF) << 8) | (instruction.dataLow & 0xFF);
	}
	else if ((instruction.addrMode & 0xF) == 0b0001) { // reg dir 3
		return findRegFromRegDescr(instruction.regsDescr & 0b1111);
	}
	else if ((instruction.addrMode & 0xF) == 0b0100) { // memorijsko 5
		return memory[((instruction.dataHigh & 0xFF) << 8) | (instruction.dataLow & 0xFF)] | (memory[(((instruction.dataHigh & 0xFF) << 8) | (instruction.dataLow & 0xFF)) + 1] << 8);
	}
	else if ((instruction.addrMode & 0xF) == 0b0010) { // reg ind 3
		return memory[findRegFromRegDescr(instruction.regsDescr & 0b1111)] | (memory[findRegFromRegDescr(instruction.regsDescr & 0b1111) + 1] << 8);
	}
	else if ((instruction.addrMode & 0xF) == 0b0101) { //reg dir pomeraj 5
		return findRegFromRegDescr(instruction.regsDescr & 0b1111) + checkSign(((instruction.dataHigh & 0xFF) << 8) | (instruction.dataLow & 0xFF));
	}
	else if ((instruction.addrMode & 0xF) == 0b0011) { //reg ind pom 5
		return memory[(findRegFromRegDescr(instruction.regsDescr & 0b1111) + (((instruction.dataHigh & 0xFF) << 8) | (instruction.dataLow & 0xFF)))] 
			| (memory[(findRegFromRegDescr(instruction.regsDescr & 0b1111) + (((instruction.dataHigh & 0xFF) << 8) | (instruction.dataLow & 0xFF))) + 1] << 8);
	}
	else {
		throw invalid_argument("Losa instrukcija");
	}
}

void Emulator::storeInOperand(Instruction instruction, int regD)
{
	if ((instruction.addrMode & 0xF) == 0b0000) { // neposredno 5
		throw invalid_argument("Ne postoji neposredni store");
	}
	else if ((instruction.addrMode & 0xF) == 0b0001) { // reg dir 3
		int& reg = findRegFromRegDescr(instruction.regsDescr & 0b1111);
		reg = regD;
	}
	else if ((instruction.addrMode & 0xF) == 0b0100) { // memorijsko 5
		memory[((instruction.dataHigh & 0xFF) << 8) | (instruction.dataLow & 0xFF)] = regD & 0xFF;
		memory[(((instruction.dataHigh & 0xFF) << 8) | (instruction.dataLow & 0xFF)) + 1] = (regD >> 8) & 0xFF;
	}
	else if ((instruction.addrMode & 0xF) == 0b0010) { // reg ind 3
		memory[findRegFromRegDescr(instruction.regsDescr & 0b1111)] = regD & 0xFF;
		memory[findRegFromRegDescr(instruction.regsDescr & 0b1111) + 1] = (regD >> 8) & 0xFF;
	}
	else if ((instruction.addrMode & 0xF) == 0b0101) { //reg dir pomeraj 5
		throw invalid_argument("Ne postoji neposredni store");
	}
	else if ((instruction.addrMode & 0xF) == 0b0011) { //reg ind pom 5
		memory[findRegFromRegDescr(instruction.regsDescr & 0b1111) + (((instruction.dataHigh & 0xFF) << 8) | (instruction.dataLow & 0xFF))] = regD & 0xFF;
		memory[(findRegFromRegDescr(instruction.regsDescr & 0b1111)) + (((instruction.dataHigh & 0xFF) << 8) | (instruction.dataLow & 0xFF)) + 1] = (regD >> 8) & 0xFF;
	}
	else {
		throw invalid_argument("Losa instrukcija");
	}
}

int& Emulator::findRegFromRegDescr(int regDescr)
{
	if (regDescr == 0) return r0;
	if (regDescr == 1) return r1;
	if (regDescr == 2) return r2;
	if (regDescr == 3) return r3;
	if (regDescr == 4) return r4;
	if (regDescr == 5) return r5;
	if (regDescr == 6) return sp;
	if (regDescr == 7) return pc;
	if (regDescr == 8) return psw;
	throw invalid_argument("Neispravan registar");
}

int Emulator::checkSign(int data)
{
	if ((data & 0x8000) == 0) {
		return data & 0xFFFF;
	}
	else {
		return data | (-1 ^ 0xFFFF);
	}
}

void Emulator::updateZ(int data)
{
	if (data == 0) psw |= 0x1;
	else psw &= 0xFFFE;
}

void Emulator::updateN(int data)
{
	if (data & 0x8000) psw |= 0x8;
	else psw &= 0xFFF7;
}

void Emulator::updateO(int data, int o1, int o2)
{
	bool isNegative = (0x8000 & data) != 0;
	if (o1 > 0 && o2 > 0 && isNegative)
	{
		psw |= 0x2;
		return;
	}
	if (o1 < 0 && o2 < 0 && !isNegative)
	{
		psw |= 0x2;
		return;
	}
	psw &= 0xFFFD;
}

string Emulator::intToBin(int data, int size)
{
	int remainder;
	string binary = "";
	int product = 1;
	while (data != 0) {
		remainder = data % 2;
		binary.insert(0, to_string(remainder));
		data = data / 2;
	}
	for (int i = 0; i < binary.size() - size; i++) {
		binary.insert(0, "0");
	}
	binary.insert(0, "0b");
	return binary;
}

string Emulator::intToHex(int data)
{
	std::stringstream ss;
	ss << std::hex << data;
	std::string result = ss.str();

	if (result.size() == 0) result.insert(0, "0000");
	else if (result.size() == 1) result.insert(0, "000");
	else if (result.size() == 2) result.insert(0, "00");
	else if (result.size() == 3) result.insert(0, "0");

	result.insert(0, "0x");
	return result;
}

void Emulator::writeStack()
{
	cout << "\n";
	if (sp == 0) return;
	for (int i = 0; i < (0xFEFE - sp); i++) {
		cout << memory[0xFEFE - 1 - i] << "<- " << 0xFEFE - 1 - i << "\n";
	}
	cout << "\n";
}

int Emulator::checkArgsLength(int argc, char* argv[])
{
	if (argc < 2) throw invalid_argument("Argumenti nisu validni");
	regex inputreg("([^ ^.]+\\.hex)");
	if (regex_match(argv[1], inputreg)) {
		return argc;
	}
	else {
		return -1;
	}
}
