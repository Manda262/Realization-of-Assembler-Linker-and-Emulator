#ifndef _emulator_h_
#define _emulator_h_

#include <iostream>
#include <regex>
#include <string>
#include <fstream>
#include <stdexcept>
#include <sstream>

using namespace std;

struct Instruction {
	int instruction;
	int instrDescr;
	int regsDescr;
	int addrMode;
	int dataHigh;
	int dataLow;
	Instruction(int ins, int s1, int s2 = 17, int s3 = 17, int s4 = NULL, int s5 = NULL) {
		instruction = ins;
		instrDescr = s1;
		regsDescr = s2;
		addrMode = s3;
		dataHigh = s4;
		dataLow = s5;
	}
};

class Emulator {
public:
	Emulator(string in);
	void start();
	static int checkArgsLength(int argc, char* argv[]);
private:

	int memory[65535] = { 0 };

	int pc;
	int psw;
	int sp;
	int sizeCode;
	int r0;
	int r1;
	int r2;
	int r3;
	int r4;
	int r5;

	bool end;

	static int MIN_ADRR;

	string input;
	void fillMemory();
	void emulate();
	void writeOut();

	Instruction instructionFetch();
	void executeInstruction(Instruction instruction);

	void executeHalt(Instruction instruction);
	void executeIret(Instruction instruction);
	void executeRet(Instruction instruction);
	void executeInt(Instruction instruction, int num);
	void executeXchg(Instruction instruction);
	void executeAdd(Instruction instruction);
	void executeSub(Instruction instruction);
	void executeMul(Instruction instruction);
	void executeDiv(Instruction instruction);
	void executeCmp(Instruction instruction);
	void executeNot(Instruction instruction);
	void executeAnd(Instruction instruction);
	void executeOr(Instruction instruction);
	void executeXor(Instruction instruction);
	void executeTest(Instruction instruction);
	void executeShl(Instruction instruction);
	void executeShr(Instruction instruction);
	void executeCall(Instruction instruction);
	void executeJmp(Instruction instruction);
	void executeJeq(Instruction instruction);
	void executeJne(Instruction instruction);
	void executeJgt(Instruction instruction);
	void executeLdrPop(Instruction instruction);
	void executeStrPush(Instruction instruction);

	int pop();
	void push(int data);
	int getValueFromOperand(Instruction instrucyion);
	void storeInOperand(Instruction instruction, int regD);
	int& findRegFromRegDescr(int regDescr);
	int checkSign(int data);

	void updateZ(int data);
	void updateN(int data);
	void updateO(int data, int o1, int o2);

	string intToBin(int data, int size);
	string intToHex(int data);

	void writeStack();
};

#endif