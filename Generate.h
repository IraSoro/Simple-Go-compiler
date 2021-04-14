#pragma once
#include <string>
#include <fstream>
#include "Tree.h"
#include "table.h"

class GenCode {
private:
	string data = "segment .data\n";
	string bss = "segment .bss\n";
	string code = "segment .code\n\tglobal _main\n\textern _printf\n";
	string _main = "";
public:
	void Generate(Tree& tree, IDTable& table) {
		vector <string> gen = table.PassForGen();
		enum en { _data = 0, _bss, _code };
		data += gen[_data];
		
		bss += gen[_bss];
		code += gen[_code];
		_main += tree.ForGen(table);
		data += tree.ForGen2(table);
		bss += tree.ForGen3(table);
		ofstream out;          
		out.open("asm.asm"); 
		if (out.is_open()){
			out << data << bss << code << _main;
		}
		out.close();

		cout << "program on asm:" << endl;
		cout << data << bss << code << _main;

		
	}
};
