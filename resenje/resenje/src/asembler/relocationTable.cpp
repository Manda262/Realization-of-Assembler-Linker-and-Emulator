#include "relocationTable.h"

void RelocationTable::addRow(int t, int off, int str)
{
	rows.push_back(RowOFRelocationTable(t, off, str));
}

void RelocationTable::writeToFile(string output)
{
	fstream outputFile;
	outputFile.open(output, fstream::in | fstream::out | fstream::app); // otvori ako postoji napravi ako ne postoji
	if (outputFile.is_open() == false) throw invalid_argument("Argumenti nisu validni!");

	for (int i = 0; i < rows.size(); i++) {
		outputFile << rows.at(i).type << ":" << rows.at(i).offset << ":" << rows.at(i).STref << "\n";
	}

	outputFile.close();
}

