#pragma once
#ifndef TABLE_H
#define TABLE_H
#include "token.h"
#include <vector>
#include <string>

struct Param {
	string NameParam;
	string TypeParam;
};

class IDTableElement {
public:
	string name;
	string kind;	//cycle_variable
	string typeVar;
	vector <string> values;
	vector <Param> param;
	string value;	//значение
	string min_value;	//0
	string max_value;	//10
	string step;		//1
	IDTableElement(string name, string kind, string value) : name(name), kind(kind), value(value) {

	}
	IDTableElement(string name, string kind, string typeVar, string value) : name(name), kind(kind), typeVar(typeVar), value(value) {}
	IDTableElement(string name, string kind, string typeVar, vector<string> values) : name(name), kind(kind), typeVar(typeVar), values(values) {}
	IDTableElement(string name, string kind, vector<Param> param) : name(name), kind(kind), param(param) {}
};

class IDTable {
private:
	unsigned int nowId = 0;
	map<unsigned int, IDTableElement> elements;
public:
	void AddRecord(string name, string kind, string typeVar = "", string value = "") {
		elements.insert(pair<unsigned int, IDTableElement>(nowId++, IDTableElement(name, kind, value)));
	}
	int AddVar(string name, string kind, string typeVar, string value) {
		elements.insert(pair<unsigned int, IDTableElement>(nowId, IDTableElement(name, kind, typeVar, value)));
		return nowId++;
	}
	void AddArray(string name, string kind, string typeVar, vector<string> values) {
		elements.insert(pair<unsigned int, IDTableElement>(nowId++, IDTableElement(name, kind, typeVar, values)));
	}
	void AddFunc(string name, string kind, vector <Param> param) {
		elements.insert(pair<unsigned int, IDTableElement>(nowId++, IDTableElement(name, kind, param)));
	}
	int FindRecordId(string name) {
		map<unsigned int, IDTableElement>::iterator it;
		it = elements.begin();
		for (it; it != elements.end(); it++) {
			if (it->second.name == name)
				return it->first;
		}
		return -1;
	} 

	string FindTypeVar(string name) {
		map<unsigned int, IDTableElement>::iterator it;
		it = elements.begin();
		for (it; it != elements.end(); it++) {
			if (it->second.name == name)
				return it->second.typeVar;
		}
		return "";
	}
	string FindKind(string name) {
		map<unsigned int, IDTableElement>::iterator it;
		it = elements.begin();
		for (it; it != elements.end(); it++) {
			if (it->second.name == name)
				return it->second.kind;
		}
		return "";
	}
	string FindName(int num) {
		map<unsigned int, IDTableElement>::iterator it = elements.begin();
		for (int i = 0; it != elements.end(); it++, i++) {
			if (i == num) {
				return it->second.name;
			}
		}
	}
	string FindNameParam(int key, int num) {
		map<unsigned int, IDTableElement>::iterator it = elements.begin();
		for (int i = 0; it != elements.end(); it++, i++) {
			if (i == key) {
				return it->second.param[num].NameParam;
			}
		}
	}
	string FindFOR(int num, bool minORmax) {
		map<unsigned int, IDTableElement>::iterator it = elements.begin();
		for (int i = 0; it != elements.end(); it++, i++) {
			if (i == num) {				
				if (minORmax) {
					return  it->second.max_value;
				}
				else return it->second.min_value;
			}
		}
	}

	vector <string> PassForGen() {
		vector <string> gen = {"", "", ""};
		enum en { data = 0, bss, code };
		string s = "123456789";
		int num = 0;
		string array = "";
		int temp = 0;
		map<unsigned int, IDTableElement>::iterator it;
		it = elements.begin();
		int k = 0;
		for (it; it != elements.end(); it++) {
			k = 0;		
			if (it->second.values.size() != 0) {
				for (int i = 0; i < it->second.values.size(); i++) {
					array += it->second.values[i];
					array += ", ";
					num++;
				}
				array.pop_back();
				array.pop_back();
			}
			if (it->second.kind == "Print") {
				if (it->second.value == "array1") {
					gen[data] += "\t";
					gen[data] += it->second.value;
					gen[data] += " dd ";
					gen[data] += array;	
					gen[data] += "\n";
					gen[data] += "\tsz dd ";
					gen[data] += s[num-1];
					gen[data] += "\n";
				}
				else {
					string str1 = it->second.value;
					string str2;
					for (int i = 0; i < str1.size(); i++) {
						if (str1[i] == '"') {
							if (k == 1) {
								if (str1.size() > 4) {
									if (str1[i + 1] == 'k' || str1[i + 1] == 'f' || str1[i + 1] == 'i' || str1[i + 1] == 'n')
										str2 += "%d\'";
									else str2 += "%f\'";
								}
								else str2 += '\'';
								k++;
							}
							else {
								str2 += '\'';
								k++;
							}
						}
						else if (k != 2) str2 += str1[i];
					}

					gen[data] += "\tstr";
					gen[data] += s[temp];
					temp++;
					gen[data] += " db ";
					gen[data] += str2;
					gen[data] += ", 0xA, 0\n";
				}
			}
			if (it->second.kind == "var" || it->second.kind == "param") {
				gen[bss] += "\t";
				gen[bss] += it->second.name;
				gen[bss] += " resd 1\n";
			}
		}
		return gen;
	}

	string PrintParam(int number) {
		map<unsigned int, IDTableElement>::iterator it;
		it = elements.begin();
		for (int i = 0; it != elements.end(); it++, i++) {
			if (i == number && it->second.kind == "Print") {
				string str1 = it->second.value;
				string str2 = "";
				int k = 0;
				for (int i = 0; i < str1.size(); i++) {
					if (str1[i] == '"') {
						if (k == 1) {
							for (int j = i+1; j < str1.size(); j++)
								str2 += str1[j];
							return str2;
						}
						else {							
							k++;
						}
					}					
				}

				return str2;
			}
		}
		return "";
	}

	void Output() {
		map<unsigned int, IDTableElement>::iterator it = elements.begin();
		cout << "_) Key _" << "	" << "name" << "		" << "kind" << "		" << "value" << "		" << "typeVar" << "		" << "min_value" << "		" << "max_value" << "		" << "step" << endl;
		for (int i = 0; it != elements.end(); it++, i++) {
			cout << i << ") Key " << it->first << "	 " << it->second.name << "		 " << it->second.kind << "		 " << it->second.value << "		 " << it->second.typeVar << "		 " << it->second.min_value << "		 " << it->second.max_value << "		 " << it->second.step << endl;
		}
	}
	void FindValue(string value, string type) {
		map<unsigned int, IDTableElement>::iterator it = elements.begin();
		for (int i = 0; it != elements.end(); it++, i++) {
			if (it->second.typeVar == value)
				it->second.typeVar = type;
		}
		return;
	}
	void FindValueToName(string name, string value) {
		map<unsigned int, IDTableElement>::iterator it = elements.begin();
		for (int i = 0; it != elements.end(); it++, i++) {
			if (it->second.name == name)
				it->second.value = value;
		}
		return;
	}

	void FindValueInFOR(string name, string min, string max, string step) {
		map<unsigned int, IDTableElement>::iterator it = elements.begin();
		for (int i = 0; it != elements.end(); it++, i++) {
			if (it->second.name == name) {
				it->second.min_value = min;
				it->second.max_value = max;
				it->second.step = step;
			}
		}
		return;
	}

};
#endif


