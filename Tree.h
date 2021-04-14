#pragma once

#include <sstream>
#include <iostream>
#include <string>
#include "token.h"
#include "table.h"


struct ExpressionElement {
	int idTableEl = -1;
	string exprEl;
	int count = 0;
};

struct TreeNode {
	TreeNode* parent = nullptr;
	string rule;		//правило
	int idTableRef;
	string data = "";
	string criterionForIF;
	string valForIF;
	vector<ExpressionElement*> expression;
	vector<TreeNode*> childs;
	TreeNode(string r, int id = -1) :rule(r), idTableRef(id) {}
	TreeNode(string r, string d) :rule(r), data(d) {}
	string GetExpression() {		//получить выражение
		if (!expression.size())
			return "";
		string expr = "";
		for (int i = 0; i < expression.size(); i++) {
			if (expression[i]->idTableEl >= 0)
				expr += "[ID:" + to_string(expression[i]->idTableEl) + "]";
			else
				expr += expression[i]->exprEl;
		}
		return expr;
	}
};

struct Stack {
	char symbol;				// Символ операции
	Stack* Next;
};

class Tree {
private:
	TreeNode* root = nullptr;
	string tab = "";
	string str = "";
	string segment_data = "";
	string segment_bss = "";
	int count = 0;
	int tempFor = 0;
	int countIF = 0;
	int countWHILE = 0;
	int countPrintf = 0;
	int temp = 0;
	bool segm = true;
	bool funcCall = false;
	string s = "0123456789";
public:
	Tree() {
	}
	~Tree() {
		ClearNode(root);
		cout << "Tree clear" << endl;
	}
	void ClearNode(TreeNode* n) { 
		if (!n)
			return;
		if (n->childs.size() == 0)
			return;
		for (int i = 0; i < n->childs.size(); i++)
			ClearNode(n->childs[i]);
		n->childs.clear();
		delete n;
	}
	TreeNode* InitRoot(string rule) {
		if (root)
			return nullptr;
		root = new TreeNode(rule);
		return root;
	}
	TreeNode* AddChild(string rule, int idTableRef = -1) {
		TreeNode* node = new TreeNode(rule, idTableRef);
		node->parent = root;
		root->childs.push_back(node);
		return root->childs.back();
	}
	
	TreeNode* AddChild(TreeNode* parent, string rule, int idTableRef = -1) {
		TreeNode* node = new TreeNode(rule, idTableRef);
		node->parent = parent;
		parent->childs.push_back(node);
		return parent->childs.back();
	}
	TreeNode* AddChild(TreeNode* parent, string rule, string data) {
		TreeNode* node = new TreeNode(rule, data);
		node->parent = parent;
		parent->childs.push_back(node);
		return parent->childs.back();
	}

	void Print() {
		tab = "";
		PrintNode(root);
	}
	void PrintNode(TreeNode* n) {
		if (!n)
			return;
		
		if (n->data == "") cout << tab << "Node:" << n->rule << " " << n->idTableRef << endl;
		else cout << tab << "Node:" << n->rule << " "  << n->data << endl;
		cout << tab << n->GetExpression() << endl;
		if (!n->childs.size()) {
			return;
		}
		tab += "\t";
		for (int i = 0; i < n->childs.size(); i++) {
			PrintNode(n->childs[i]);
		}
		tab.erase(tab.size() - 1, 1);  
	}
	TreeNode* ToParent(TreeNode* node) {
		return node->parent;
	}
	TreeNode* ToRoot() {
		return root;
	}
	void AddExprEl(TreeNode* node, int el) {
		ExpressionElement* newEl = new ExpressionElement();
		newEl->idTableEl = el;
		node->expression.push_back(newEl);
	}
	void AddExprEl(TreeNode* node, string el) {
		ExpressionElement* newEl = new ExpressionElement();
		newEl->exprEl = el;
		node->expression.push_back(newEl);
	}

	vector <string> startOPZ(vector <string> In) {
		Stack *Op = NULL;										// Стек операций Op – пуст
		char temp = ' ', a = ' '; 
		vector <string> Out;												// Входная In и выходная Out строки
		int k = 0, l = 0, n = 0;										// Текущие индексы для строк
		for (int i = 0; i < In.size(); i++)
			cout << In[i] << " " ;
		cout << endl;

		if (In[0] == "-") {
			Out.push_back(In[1]);
			Out.push_back(In[0]); 
			if (In.size()>2) {
				Out.push_back(In[4]);
				Out.push_back(In[3]);
				Out.push_back(In[2]);
			}
			cout << "OPZ =" << " ";													// Выводим на экран результат
			for (int i = 0; i < Out.size(); i++)
				cout << Out[i] << " ";
			cout << endl;
			return Out;
		}
		
		while (k < In.size()) {								// Анализируем символы строки In

			if (In[k].size() > 1 ) {
				n++;
				Out.push_back(In[k]);
				l++;
				if (n > 1) break;
			}
			else {
				temp = In.at(k)[0];
				if ((temp >= 'a' && temp <= 'z') || (temp >= 'A' && temp <= 'Z') || (temp >= '0' && temp <= '9') ) { Out.push_back(In[k]); l++; }		// Если символ строки In – буква, заносим ее в строку Out													
			}
			if (temp == '+' || temp == '-' || temp == '*' || temp == '/' ) {	//Если символ – знак операции, 
				cout << "temp = " << temp << endl;
				while (Op != NULL && Prior(Op->symbol) >= Prior(temp)) {		//переписываем из стека в строку Out все операции с большим или равным приоритетом 
					Op = OutStack(Op, &a);										// Извлекаем из стека символ 
					string str = "";											// и записываем в строку Out
					str += a;
					cout << "str = " << str << endl;
					Out.push_back(str);
					l++;
				}
				Op = InStack(Op, temp);											// Текущий символ – в стек
				
			}
			k++;
		}																			// Конец цикла анализа входной строки
		while (Op != NULL) {														// Если стек не пуст, переписываем все операции в выходную строку
			Op = OutStack(Op, &a);
			string str = "";
			str += a;
			Out.push_back(str);
			l++;
		}
		cout << "OPZ =" << " ";													// Выводим на экран результат
		for (int i = 0; i < Out.size(); i++)
			cout << Out[i] << " " ;
		cout << endl;
		return Out;
	}

	int Prior(char a) {													//Функция реализации приоритета операций 
		switch (a) {
		case '*': case '/': return 3;
		case '-': case '+': return 2;
		case '^': return 1;
		}
		return 0;
	}

	Stack* InStack(Stack* t, char s) {										// Добавление элемента в стек
		Stack* t1 = (Stack*)malloc(sizeof(Stack));
		t1->symbol = s;
		t1->Next = t;
		return t1;
	}

	Stack* OutStack(Stack* t, char* s) {									// Извлечение элемента из стека 
		Stack* t1 = t;
		*s = t->symbol;
		t = t->Next;
		free(t1);
		return t;
	}

	void StartGen(IDTable& table) {
		int k = 0;
		for (int i = 0; i < root->childs.size(); i++) {
			if (root->childs[i]->rule == "func main") {
				str += "_main:\n";
				GenNode(table, root->childs[i]);
				k = i+1;
				break;
			}
		}
		if (k < root->childs.size() && root->childs[k]->rule == "func binary_sorting") {
			str += "_binary_sorting:\n";
			for (int j = 2; j >= 0; j--) {
				str += "\tmov eax, [esp+";
				str += FromIntToString((3-j)*4);
				str += "]\n";
				str += "\tmov dword[";
				str += table.FindNameParam(7, j);
				str += "], eax\n";		
			}

			segment_bss += "\tprint_i resd 1\n";
			GenNode(table, root->childs[k]);
			str += "\tret\n";
		}
		return;
	}

	void GenNode(IDTable& table, TreeNode* n) {
		int k = 0;
		for (int i = 0; i < n->childs.size(); i++) {
			if (n->childs[i]->parent->rule == "func binary_sorting"){
				//cout << "i = " << n->childs.size() << endl;
			}
			if (n->childs[i]->rule == "function call") {
				funcCall = true; 
				for (int j = 0; j < n->childs[i]->childs.size(); j++) {
					if (n->childs[i]->childs[j]->rule == "id" && table.FindKind(table.FindName(n->childs[i]->childs[j]->idTableRef)) == "array") {						
						str += "\tpush ";
						str += table.FindName(n->childs[i]->childs[j]->idTableRef);
						str += "\n";
					}
					else  if (n->childs[i]->childs[j]->rule == "id" && (table.FindKind(table.FindName(n->childs[i]->childs[j]->idTableRef)) == "var" || table.FindKind(table.FindName(n->childs[i]->childs[j]->idTableRef)) == "param")) {
						str += "\tpush dword[";
						str += table.FindName(n->childs[i]->childs[j]->idTableRef);
						str += "]\n";
					}
					else {
						str += "\tpush 0\n";
					}
				}
				str += "\tcall _binary_sorting\n";
				str += "\tadd esp, ";
				str += FromIntToString(n->childs[i]->childs.size()*4);
				str += "\n";
			}else
			if (n->childs[i]->rule == "equal" && n->childs[i]->expression.size() > 1 && n->childs[i]->parent->rule != "condition FOR" && table.FindTypeVar(table.FindName(n->childs[i]->idTableRef)) != "int") {				
				
				str += ActionForFloat(n->childs[i]->idTableRef, table, n->childs[i]->expression);
				
			} else
			if (n->childs[i]->rule == "equal" && n->childs[i]->expression.size() > 1 && n->childs[i]->parent->rule != "condition FOR" && table.FindTypeVar(table.FindName(n->childs[i]->idTableRef)) == "int") {
				//cout << "flag = " <<funcCall<< endl;
				str += ActionForInt(n->childs[i]->idTableRef, table, n->childs[i]->expression);
					
			}else
			if (n->childs[i]->rule == "equal" && n->childs[i]->expression.size() == 1 && n->childs[i]->parent->rule != "condition FOR" && n->childs[i]->parent->rule != "id") {
				if (funcCall) {
					if (n->childs[i]->expression[0] > 0) {
						if (table.FindName(n->childs[i]->expression[0]->idTableEl) == "array") {
							str += "\tmov byte[val], al\n";
							str += "\tmov edi, dword[val]\n";
							str += "\tmov ebx, dword[";
							str += table.FindName(n->childs[i]->expression[0]->idTableEl);
							str += "]\n";
							str += "\tmov eax, dword[ebx+4*edi]\n";
							str += "\tmov dword[";
							str += table.FindName(n->childs[i]->idTableRef);
							str += "], eax\n";

						}
						else {
							str += "\tmov eax, dword[";
							str += table.FindName((n->childs[i]->expression[0]->idTableEl));
							str += "]\n";
							str += "\tmov dword[";
							str += table.FindName(n->childs[i]->idTableRef);
							str += "], eax\n";
						}
					} 
				}else
				if (table.FindTypeVar(table.FindName(n->childs[i]->idTableRef)) == "int") {
					str += "\tmov dword[";
					str += table.FindName(n->childs[i]->idTableRef);
					str += "], ";
					str += n->childs[i]->expression[0]->exprEl;
					str += "\n";
				}
				else {
					str += "\tmov dword[";
					str += table.FindName(n->childs[i]->idTableRef);
					str += "], __float32__(";
					str += n->childs[i]->expression[0]->exprEl; 
					str += ".0)\n";
				}
			} else
			if (n->childs[i]->rule == "condition FOR") {
				tempFor++;
				str += "\tmov dword[";
				k = n->childs[i]->idTableRef;
				str += table.FindName(k);
				str += "], ";
				bool flag = 0;
				str += table.FindFOR(k, flag);
				str += "\n";
				str += "\t_cycle";
				str += s[tempFor];
				str += ":\n";
				GenNode(table, n->childs[i]);
			} else
			if (n->childs[i]->rule == "condition WHILE") {
				temp = 1;
				cout << "WHILE" << endl;
				countWHILE++;
				if (countWHILE == 1) {
					str += "\tjmp _cycle1\n";
					GenNode(table, n->childs[i]);
				}
				else if (countWHILE == 2) {
					str += "\tjmp _cycle2\n";
					str += "\t_cycle2:\n";
					str += "\t_cycle2_condition:\n";
					GenNode(table, n->childs[i]);
					
					str += "\tjmp _cycle2\n";
					str += "\t_cycle2_end:\n";
				}
				else if (countWHILE == 3) {
					
					str += "\tjmp _cycle3\n";
					str += "\t_cycle3:\n";
					str += "\t_cycle3_condition:\n";
					GenNode(table, n->childs[i]);
					str += "\tjmp _cycle3\n";
					str += "\t_cycle3_end:\n";
					countWHILE = 0;
				}
				
				//GenNode(table, n->childs[i]);

			} else
			if (n->childs[i]->rule == "id" && n->childs[i]->parent->rule == "condition WHILE") {
				bool k = 0;
				for (int j = 0; j < n->childs[i]->expression.size(); j++) {
					
					if (countWHILE == 1) {
						str += "\t_cycle1:\n";
						str += "\t_cycle1_condition:\n";
						str += "\tmov eax, dword[";
						str += table.FindName(n->childs[i]->idTableRef);
						str += "]\n";
						str += "\tcmp eax, dword[";
						str += table.FindName(n->childs[i]->expression[1]->idTableEl);
						str += "]\n";
						str += "\tjle _cycle1_block\n";
						str += "\tjmp _cycle1_end\n";
						str += "\t_cycle1_block:\n";
						break;
					}else
					if (n->childs[i]->expression[j]->idTableEl == -1 && (n->childs[i]->expression[j]->exprEl == "[" || n->childs[i]->expression[j]->exprEl == "]")) {
						
					}
					else
					if (n->childs[i]->expression[j]->idTableEl != -1) {
						if (!k) {
							//cout << "567890" << endl;
							str += "\tmov edi, dword[";
							str += table.FindName(n->childs[i]->expression[j]->idTableEl);
							str += "]\n";
							str += "\tmov ebx, dword[";
							str += table.FindName(n->childs[i]->idTableRef);
							str += "]\n";
							str += "\tmov eax, dword[ebx+4*edi]\n";
							k = 1;
						}
						else {
							//cout << "1111111" << endl;
							str += "\tcmp eax, dword[";
							str += table.FindName(n->childs[i]->expression[j]->idTableEl);
							str += "]\n";
						}
					}
				}
				GenNode(table, n->childs[i]);

			}else
			if (n->childs[i]->rule == "id" && n->childs[i]->parent->rule == "condition IF" && (n->childs[i]->parent->parent->rule == "condition WHILE" || n->childs[i]->parent->parent->rule == "func binary_sorting" || n->childs[i]->parent->parent->rule == "id")) {
				for (int j = 0; j < n->childs[i]->expression.size(); j++) {
					if (n->childs[i]->expression[j]->exprEl == "<=") {
						
						str += "\tmov eax, dword[";
						str += table.FindName(n->childs[i]->idTableRef);
						str += "]\n";
						str += "\tcmp eax, dword[";
						str += n->childs[i]->expression[j + 1]->exprEl;
						str += "]\n";
						str += "\tjle _if1_cond1_block\n";
						str += "\tjmp _if1_end\n";
						str += "\t_if1_cond1_block:\n";

						GenNode(table, n->childs[i]);
						break;
					}
					else
					if (n->childs[i]->expression[j]->exprEl == "<") {
						str += "\tmov eax, dword[";
						str += table.FindName(n->childs[i]->idTableRef);
						str += "]\n";
						str += "\tcmp eax, dword[";
						str += n->childs[i]->expression[j + 1]->exprEl;
						str += "]\n";
						str += "\tjl _if2_cond_block\n";
						str += "\tjmp _if2_end\n";
						str += "\t_if2_cond_block:\n";
						GenNode(table, n->childs[i]);
						str += "\t_if2_end:\n";
						break;

					}else if(n->childs[i]->expression[j]->exprEl == ">") {
						str += "\tmov eax, dword[";
						str += table.FindName(n->childs[i]->idTableRef);
						str += "]\n";
						str += "\tcmp eax, dword[";
						str += n->childs[i]->expression[j + 1]->exprEl;
						str += "]\n";
						str += "\tjg _if3_cond_block\n";
						str += "\tjmp _if3_end\n";
						str += "\t_if3_cond_block:\n";
						GenNode(table, n->childs[i]);
						str += "\t_if3_end:\n";
						break;

					}
				}
			}else
			if (n->childs[i]->rule == "equal" && n->childs[i]->parent->rule == "id" && countIF>0) {
				str += ActionForInt(n->childs[i]->idTableRef, table, n->childs[i]->expression);
			}
			else
			if (n->childs[i]->rule == "equal" && n->childs[i]->parent->rule == "condition FOR") {
				str += ActionFOR(n->childs[i]->idTableRef, table, n->childs[i]->expression);
				if (i == n->childs.size() - 1) {
					str += "\t\tinc dword[i]\n";
					str += "\t\tmov eax, dword[i]\n";
					str += "\t\tcmp eax, dword[";
					str += table.FindFOR(table.FindRecordId("i"), 1);
					str += "]\n";
					str += "\t\tjle _cycle1\n";
				}
				GenNode(table, n->childs[i]);
			}else
			if (n->childs[i]->rule == "condition IF" && (n->childs[i]->parent->rule == "id" || n->childs[i]->parent->rule == "func binary_sorting")) {
				countIF++;
				if (countIF == 1) {
					str += "\tjmp _if1_cond1\n";
					str += "\t_if1_cond1:\n";
					GenNode(table, n->childs[i]);
					
					str += "\tjmp _if1_end\n";
					str += "\t_if1_end:\n";
					str += "\tjmp _cycle1\n";
					str += "\t_cycle1_end:\n";
				}else 
				if (countIF == 2) {
					str += "\tjmp _if2_cond\n";
					str += "\t_if2_cond:\n";
					GenNode(table, n->childs[i]);

				}else
				if (countIF == 3) {
					str += "\tjmp _if3_cond\n";
					str += "\t_if3_cond:\n";
					GenNode(table, n->childs[i]);
				}

				
				
				
			}else
			if (n->childs[i]->rule == "condition IF" || n->childs[i]->rule == "else") {
				if (n->childs[i]->childs[0]->rule != "condition IF")
					countIF++;
				GenNode(table, n->childs[i]);
				
			} else
			if (n->childs[i]->rule == "id" && (n->childs[i]->parent->rule == "condition IF" || n->childs[i]->parent->rule == "else")) {
				string con = "";
				for (int j = 0; j < n->childs[i]->expression.size(); j++) {
					if (n->childs[i]->expression[j]->exprEl[0] >= '0' && n->childs[i]->expression[j]->exprEl[0] <= '9') {
						count++;
						segment_data += "\tconst";
						con += "const";
						segment_data += FromIntToString(count);
						con += FromIntToString(count);
						segment_data += " dd ";
						segment_data += n->childs[i]->expression[j]->exprEl[0];
						segment_data += ".0\n";
					}
				}
				if (countIF == 1) {
					str += "\tmov eax, dword[";
					str += con;
					str += "]\n";
					str += "\tcmp dword[";
					str += table.FindName(n->childs[i]->idTableRef);
					str += "], eax\n";
					str += "\tjg _if1_cond1\n";
					str += "\tje _if1_cond2\n";
					str += "\tjmp _if1_cond3\n";
					str += "\t_if1_cond1:\n";
					GenNode(table, n->childs[i]);
				} else
				if (countIF == 2) {
					str += "\tjmp _if1_endif\n";
					str += "\t_if1_cond2:\n";
					GenNode(table, n->childs[i]);
				} else
				if (countIF == 3) {
					str += "\t_if1_cond3:\n";
				}


			}else
			if (table, n->childs[i]->rule == "Print" && n->childs[i]->parent->rule != "id" && n->childs[i]->parent->rule != "else") {
				if (n->childs[i - 1]->rule == "function call") {
					str += "\tmov dword[print_i], 0\n";
					str += "\t\t_print_arr_cycle:\n";
					str += "\t\tmov edi, dword[print_i]\n";
					str += "\t\tmov eax, [";
					str += table.FindName(n->childs[i-1]->childs[0]->idTableRef);
					str += "+4*edi]\n";
					str += "\t\tpush eax\n";
					segment_data += "\tres db '%d', 0xA, 0\n";
					segment_data += "\tstr1 db '%d ', 0\n";
					str += "\t\tpush str1\n";
					str += "\t\tcall _printf\n";
					str += "\t\tadd esp, 8\n";
					str += "\t\tinc dword[print_i]\n";
					str += "\t\tmov eax, dword[print_i]\n";
					str += "\t\tcmp eax, dword[sz]\n";
					str += "\t\tjl _print_arr_cycle\n";
					str += "\tret\n";

				}
				else {
					str += "\tpush dword[";
					str += table.PrintParam(n->childs[i]->idTableRef);
					str += "]\n";
					str += "\tpush str1\n";
					str += "\tcall _printf\n";
					
					str += "\tadd esp, 8\n";
					str += "\tret\n";
				}
			}
			else 
			if (n->childs[i]->rule == "Print" && (n->childs[i]->parent->rule == "id" || n->childs[i]->parent->rule == "else")) {
				if (countIF == 3) {
					str += "\t_if1_cond3:\n";
				}
				countPrintf++;
				if (countPrintf < 4) {
					//cout << "Printf = " << countPrintf << endl;
					str += "\tsub esp, 8\n";
					str += "\tfld dword[";
					if (countPrintf == 1 || countPrintf == 3) {
						str += "x1]\n";
						str += "\tfstp qword[esp]\n";
						str += "\tpush str";
						str += FromIntToString(countPrintf);
						str += "\n";						
					}
					else if (countPrintf == 2) {
						str += "x2]\n";
						str += "\tfstp qword[esp]\n";
						str += "\tpush str";
						str += FromIntToString(countPrintf);
						str += "\n";
					}
					str += "\tcall _printf\n";
					str += "\tadd esp, 12\n";
					if (countPrintf == 3) {
						str += "\tjmp _if1_endif\n";
					}
				}
				else if (countPrintf == 4) {
					str += "\tpush str4\n";
					str += "\tcall _printf\n";
					str += "\tadd esp, 4\n";
					str += "\tjmp _if1_endif\n";
					str += "\t_if1_endif:\n";
					str += "\tret\n";
				}
			}/*else
				if (n->childs[i]->rule == "else") {
					countIF++;
					GenNode(table, n->childs[i]);
				}*/
		}
		//cout << str << endl;
		return ;
	}

	string ForGen(IDTable& table) {
		StartGen(table);
		return str;
	}
	string ForGen2(IDTable& table) {
		//StartGen(table);
		return segment_data;
	}
	string ForGen3(IDTable& table) {
		//StartGen(table);
		return segment_bss;
	}
	
	string ActionForInt(int number, IDTable& table, vector <ExpressionElement*> ac) {
		string st = "";
		for (int i = 0; i < ac.size(); i++) {
			if (funcCall == true) {
				if (countWHILE == 2) {
					st += "\tjg _cycle2_block\n";
					st += "\tjmp _cycle2_end\n";
					st += "\t_cycle2_block:\n";
					st += "\tinc dword[left]\n";
					break;
				}
				else if (countWHILE == 3) {
					st += "\tjl _cycle3_block\n";
					st += "\tjmp _cycle3_end\n";
					st += "\t_cycle3_block:\n";
					st += "\tdec dword[right]\n";
					break;
				}
				else
				if (countIF == 1) {
					
					if (temp == 1) {
						st += "\tmov edi, dword["; 
						st += "left";
						st += "]\n";
						st += "\tmov ebx, dword[";
						st += table.FindName(8);
						st += "]\n";
						st += "\tmov eax, dword[ebx+4*edi]\n";
						st += "\tmov dword[";
						st += "temp";
						st += "], eax\n";
					}else
					if (temp == 2) {
						st += "\tmov edi, dword[";
						st += "right";
						st += "]\n";
						st += "\tmov ebx, dword[";
						st += table.FindName(8);
						st += "]\n";
						st += "\tmov eax, [ebx+4*edi]\n";
						st += "\tmov edi, dword[left]\n";
						st += "\tmov ebx, dword[array]\n";
						st += "\tmov [ebx+4*edi], eax\n";
					}else
					if (temp == 3) {
						st += "\tmov edi, dword[";
						st += "right";
						st += "]\n";
						st += "\tmov ebx, dword[";
						st += table.FindName(8);
						st += "]\n";
						st += "\tmov eax, dword[temp]\n";
						st += "\tmov [ebx+4*edi], eax\n";

					}else
					if (temp == 4) {
						st += "\tinc dword[left]\n";
						
					}else
					if (temp == 5) {
						
						st += "\tdec dword[right]\n";
					}
					temp++;
					break;
				}
				else
				if (ac[i]->idTableEl != -1) {
					if (ac[i+1]->exprEl == "+")
						st += "\tadd eax, dword["; else
					st += "\tmov eax, dword[";
					st += table.FindName(ac[i]->idTableEl);
					st += "]\n";
				}else
				if (ac[i]->idTableEl == -1) {
					if (ac[i]->exprEl == "+") {
						st += "\tmov dword[";
						st += table.FindName(number);
						st += "], eax\n";
					}else
					if (ac[i]->exprEl == "/") {
						st += "\tdiv bh\n";
					} else
					if (ac[i]->exprEl[0] >= '0' && ac[i]->exprEl[0] <= '9') {
						st += "\tmov bh, ";
						st += ac[i]->exprEl[0];
						st += "\n";
					}
				}
			
			}else
			if (ac[i]->idTableEl != -1 ) {
				st += "\tmov eax, dword[";
				st += table.FindName(ac[i]->idTableEl);
				st += "]\n";
				
			}
			else if (ac[i]->idTableEl == -1) {
				if (ac[i+1]->exprEl == "-") {
					st += "\tsub eax, ";
					st += ac[i]->exprEl[0];
					st += "\n";
					i++;
				}	

			}
		}
		//cout << "int" << endl;
		if (!funcCall) {
			st += "\tmov dword[";
			st += table.FindName(number);
			st += "], eax\n";
		}

		return st;
	}

	string ActionForFloat(int number, IDTable& table, vector <ExpressionElement*> ac) {
		string st = "";
		for (int i = 0; i < ac.size(); i++) {
			if (ac[i]->idTableEl != -1 && ac[i+1]->exprEl != "/") {
				st += "\tfld dword[";
				st += table.FindName(ac[i]->idTableEl );
				st += "]\n";

			}else
			if (ac[i]->idTableEl == -1 ) {
				if (ac[i]->exprEl == "+") {
					st += "\tfadd\n";
				}else
				if (ac[i]->exprEl == "*") {
					st += "\tfmul\n";
				}else
				if (ac[i]->exprEl == "-") {
					if (i == 1) {
						if (segm) {
							segment_data += "\tminus_one dd -1.0\n";
							segm = false;
						}
						st += "\tfld dword[minus_one]\n";
						st += "\tfmul\n";
					}else
					st += "\tfsub\n";
				}else
				if (ac[i]->exprEl[0] >= '0' && ac[i]->exprEl[0] <= '9' && ac[i + 1]->exprEl != "/") {
					count++;
					segment_data += "\tconst";
					segment_data += FromIntToString(count);
					segment_data += " dd ";
					segment_data += ac[i]->exprEl[0];
					segment_data += ".0\n";
					st += "\tfld dword[const";
					st += FromIntToString(count);
					st += "]\n";
				}else
				if (ac[i]->exprEl == "math.Sqrt") {
					st += "\tfsqrt\n";
				}else
				if (ac[i]->exprEl == "/") {
					st += "\tfdiv dword[";
					if (ac[i-1]->exprEl[0] >= '0' && ac[i]->exprEl[0] <= '9') {
						count++;
						segment_data += "\tconst";
						segment_data += FromIntToString(count);
						segment_data += " dd ";
						segment_data += ac[i-1]->exprEl[0];
						segment_data += ".0\n";
						st += "const";
						st += FromIntToString(count);

					}
					else {
						st += table.FindName(ac[i-1]->idTableEl);
					}
					st += "]\n";
				}

			}
		}
		st += "\tfstp dword[";
		st += table.FindName(number);
		st += "]\n";
		
		return st;
	}

	string ActionFOR(int number, IDTable& table, vector <ExpressionElement*> ac) {
		string st = "";
		int n = 0;
		if (ac.size() == 1) {
			st += "\t\tmov eax, dword[";
			st += table.FindName(ac[0]->idTableEl);
			st += "]\n";
			st += "\t\tmov dword[";
			st += table.FindName(number);
			st += "], eax\n";
			return st;
		}
		for (int j = 0; j <ac.size(); j++) {
			if (ac[j]->idTableEl > 0) {
				n++;
				ac[j]->count = n;
				st += "\t\tpush dword[";
				st += table.FindName(ac[j]->idTableEl);
				st += "]\n";
			}
			else if (ac[j]->idTableEl == -1) {
				st += "\t\tmov eax, dword[esp+4]\n";
				if (ac[j]->exprEl == "*") {
					st += "\t\timul dword[esp]\n";
				} if (ac[j]->exprEl == "+") {
					st += "\t\tadd eax, dword[esp]\n";					
				}
				st += "\t\tmov dword[";
				st += table.FindName(number);
				st += "], eax\n";
				st += "\t\tadd esp, ";
				st += FromIntToString(n * 4);
				st += "\n";				
			}

		}

		return st;
	}

	string FromIntToString(int val) {
		stringstream a1;
		string str;
		a1 << val;
		a1 >> str;
		return str;
	}
};



