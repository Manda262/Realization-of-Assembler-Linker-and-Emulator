#ifndef _relocationTable_h_
#define _relocationTable_h_

#include <vector>
#include <string>
#include <fstream>

using namespace std;

struct RowOFRelocationTable {
	int type;
	int offset;
	int STref;
	RowOFRelocationTable(int t, int off, int str) {
		type = t;
		offset = off;
		STref = str;
	}
};

class RelocationTable {
public:
	RelocationTable(string sec) { section = sec; }
	void addRow(int t, int off, int str);
	string getSection() { return section; }
	void writeToFile(string output);
	int getSize() { return rows.size(); }
private:
	vector<RowOFRelocationTable> rows;
	string section;
};

#endif // !_relocationTable_h_
