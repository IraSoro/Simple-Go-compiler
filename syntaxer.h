#ifndef SYNTAXER_H
#define SYNTAXER_H

#include "token.h"
#include "table.h"
#include "Tree.h"
#include "Generate.h"


using namespace std;

class Syntaxer {
private:
	IDTable table;
	vector <string> array;
	Tree tree;
	vector <string> action;
	vector <string> OutAction;
	bool yes = false;
	TreeNode* nowNode;
	vector <Param> param;
	int first = 0;
	int second = 0;
	int temp = 0;
	vector<Token>* tokens;
	vector<Token>::const_iterator nowToken;
	vector<Token>::const_iterator saveToken;
	GenCode gen;
	bool Next() {
		nowToken++;
		if (nowToken == tokens->end()) {
			return false;
		}
		return true;
	}
	bool Prev() {
		nowToken--;
		if (nowToken == tokens->end()) {
			return false;
		}
		return true;
	}
	void Save() {
		saveToken = nowToken;
	}
	void Load() {
		nowToken = saveToken;
	}

	bool Init() {
		nowNode = tree.InitRoot("Init");														//!!!!!!!!!!!!
		if (!Package())
			return false;
		else
			if (!Import())
				return false;
			else
				if (!Func())
					return false;

		if (Next())
			if (!FuncDefinition())
				return false;
		table.Output();
		return true;
	}
	bool Package() {
		// PACKAGE:= KWORD(package) ID(main)
		if (nowToken->type != TokenType::KEYWORDS || nowToken->value != "package")
			return false;
		string type = nowToken->value; //package
		if (!Next())
			return false;
		if (nowToken->type != TokenType::ID || nowToken->value != "main")
			return false;
		string name = nowToken->value; //main
		if (!Next())
			return false;
		if (nowToken->type != TokenType::NEWLINE)
			return false;
		if (!Next())
			return false;
		cout << "Found package" << endl;
		table.AddRecord(name, type);														//!!!!!!!!!!!!!!!!!
		tree.AddChild(nowNode, "Package");														//!!!!!!!!!!!!!!!!!
		return true;
	}
	bool Import() {
		// IMPORT := KEYWORD(import) STRING
		if (nowToken->type != TokenType::KEYWORDS || nowToken->value != "import")
			return false;
		string type = nowToken->value;
		if (!Next())
			return false;
		if (nowToken->type != TokenType::STRING)
			return false;
		string name = nowToken->value;
		if (!Next())
			return false;
		if (nowToken->type != TokenType::NEWLINE)
			return false;
		if (!Next())
			return false;
		cout << "Found import" << endl;
		table.AddRecord(name, type);													//!!!!!!!
		tree.AddChild(nowNode, "Import");													//!!!!!!!
		Save();
		if (!Import())
			Load();
		return true;
	}
	bool Func() {
		if (!FuncDeclaration())
			return false;
		else if (!OpenBracket())
			return false;
		else if (!FuncVar())
			return false;
		else if (!Action())
			return false;
		else if (!CauseFunc()) {
			if (!ConditionIF()) {
				if (!ConditionFOR())
					return false;
			}
		}
		if (!Print()) {
			if (!CloseBracketEnd())
				return false;
		}
		else if (!CloseBracketEnd())
			return false;


		return true;
	}
	bool FuncDeclaration() {
		// !!!!!!!!!!!!!!!!!!!!!!!
		if (nowToken->type != TokenType::KEYWORDS || nowToken->value != "func")
			return false;
		string kind = nowToken->value;
		if (!Next())
			return false;
		if (nowToken->type != TokenType::ID || nowToken->value != "main")
			return false;
		string name = nowToken->value;
		if (!Next())
			return false;
		if (nowToken->type != TokenType::DELIM || nowToken->value != "(")
			return false;
		if (!Next())
			return false;
		if (nowToken->type != TokenType::DELIM || nowToken->value != ")")
			return false;
		if (!Next())
			return false;

		cout << "Found FUNC MAIN()" << endl;
		table.AddRecord(name, kind);																	//!!!!!
		nowNode = tree.AddChild(nowNode, "func main");															//!!!!!
		return true;
	}
	bool FuncVar() {

		while (nowToken->type == TokenType::NEWLINE)
			nowToken++;

		if (nowToken->type != TokenType::KEYWORDS || nowToken->value != "var")
			return false;
		nowNode = tree.AddChild(nowNode, "var declare");
		if (!Next())
			return false;

		nowToken++;
		if (nowToken->value != "=") {
			nowToken--;
			if (!DeclarationVar())
				return false;
		}
		else {
			nowToken--;
			string kind = "array";
			if (nowToken->type != TokenType::ID)
				return false;
			string name = nowToken->value;
			if (!Next())
				return false;

			if (!(nowToken->type == TokenType::DELIM || nowToken->value == "="))
				return false;
			if (!Next())
				return false;

			if (!(nowToken->type == TokenType::DELIM || nowToken->value == "["))
				return false;
			if (!Next())
				return false;

			if (!(nowToken->type == TokenType::DELIM || nowToken->value == "]"))
				return false;
			if (!Next())
				return false;

			if (!(nowToken->type == TokenType::KEYWORDS || nowToken->value == "int"))
				return false;
			string typeVar = nowToken->value;
			if (!Next())
				return false;

			if (!(nowToken->type == TokenType::DELIM || nowToken->value == "{"))
				return false;
			if (!Next())
				return false;

			if (nowToken->type != TokenType::NUM)
				return false;
			array.push_back(nowToken->value);
			if (!Next())
				return false;

			if (!Array())
				return false;

			cout << "______________" << endl;
			for (int i = 0; i < array.size(); i++)
				cout << array[i] ;
			cout << "_____________" << endl;

			if (!(nowToken->type == TokenType::DELIM || nowToken->value == "}"))
				return false;
			if (!Next())
				return false;

			table.AddArray(name, kind, typeVar, array);
			tree.AddChild(nowNode, "id", table.FindRecordId(name));				//!!!!!!!!!!
		}
		nowNode = tree.ToParent(nowNode);							//!!!!!!!!!!!!
		Save();
		if (!FuncVar())
			Load();
		
		cout << "Found var..." << endl;
		
		return true;
	}
	bool DeclarationVar() {
		string kind = "var";
		if (nowToken->type != TokenType::ID)
			return false;
		string name = nowToken->value;
		if (!Next())
			return false;

		if (!(nowToken->type == TokenType::DELIM || nowToken->value == "," || nowToken->type == TokenType::KEYWORDS))
			return false;
		string typeVar = nowToken->value;
		if (nowToken->type != TokenType::KEYWORDS) {
			typeVar = "later";															//!!!!!!
		}
		if (!Next())
			return false;

		if (nowToken->type == TokenType::NEWLINE) {
			//table.AddVar(name, kind, typeVar, "None");											//!!!!!			
																//!!!!!
			tree.AddChild(nowNode, "id", table.AddVar(name, kind, typeVar, "None"));				//!!!!!!!!!!
			table.FindValue("later", typeVar);
			if (!Next())
				return false;
			return true;
		}
		
		tree.AddChild(nowNode, "id", table.AddVar(name, kind, typeVar, "None"));				//!!!!!!!!!!
		Save();
		if (!DeclarationVar())
			Load();
		
		return true;
	}
	bool Array() {
		if (!(nowToken->type == TokenType::DELIM || nowToken->value == ","))
			return false;
		if (!Next())
			return false;

		if (nowToken->type != TokenType::NUM)
			return false;
		array.push_back(nowToken->value);
		if (!Next())
			return false;

		Save();
		if (!Array())
			Load();
	}
	bool Action() {
		while (nowToken->type == TokenType::NEWLINE)
			nowToken++;
		action.clear();													//------------------------------
		yes = false;
		string name;
		if (nowToken->type != TokenType::ID && nowToken->value != "fmt")
			return false;
		if (nowToken->type == TokenType::ID) {
			name = nowToken->value;
		}
		if (!Next())
			return false;

		if (nowToken->value == "[") {
			if (!Next())
				return false;

			if (nowToken->type != TokenType::ID)
				return false;
			if (!Next())
				return false;

			if (!(nowToken->value == "]" && nowToken->type == TokenType::DELIM))
				return false;
			if (!Next())
				return false;

		}

		if (nowToken->type == TokenType::MATH) {
			action.push_back(name);
			string str = nowToken->value;
			yes = true;
			if (!Next())
				return false;
			nowNode = tree.AddChild(nowNode, "equal", table.FindRecordId(name));					//!!!!!!!!!
			//tree.AddExprEl(nowNode, str);															//!!!!!!!!!
			cout << "found action with ++ or --" << endl;
			string s;
			s += str[0];
			action.push_back(s);																	//-------------
			s = ""; s += '1';
			action.push_back(s);																	//-------------
			
			
			if (yes) {
				cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
				OutAction = tree.startOPZ(action);															//------------
				cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
			}
			
			for (int i = 0; i < OutAction.size(); i++) {
				if (OutAction[i].size() > 1 && OutAction[i] != "math.Sqrt") {
					tree.AddExprEl(nowNode, table.FindRecordId(OutAction[i]));
				}
				else
					if ((OutAction.at(i)[0] >= 'a' && OutAction.at(i)[0] <= 'z') || (OutAction.at(i)[0] >= 'A' && OutAction.at(i)[0] <= 'Z')) {
						tree.AddExprEl(nowNode, table.FindRecordId(OutAction[i]));
					}
					else {
						tree.AddExprEl(nowNode, OutAction.at(i));
					}
			}
			nowNode = tree.ToParent(nowNode);														//!!!!!!!
			yes = false;
			Save();
			if (!Action())
				Load();
			
			if (nowToken->type == TokenType::NEWLINE)
				nowToken++;
			
			return true;
		}

		if (!(nowToken->type == TokenType::DELIM && nowToken->value == "="))
			return false;
		nowNode = tree.AddChild(nowNode, "equal", table.FindRecordId(name));					//!!!!!!!!!
		if (!Next())
			return false;

		if (nowToken->value == "-") {			
			yes = true;
			string sqrt = "";
			action.push_back(nowToken->value);													//-------------
			cout << "found -ID" << endl;
			//tree.AddExprEl(nowNode, "-");														//!!!!!!!!!
			if (!Next())
				return false;
			if (nowToken->type != TokenType::ID)
				return false;
			action.push_back(nowToken->value);													//-------------
			table.FindRecordId(nowToken->value);
			//tree.AddExprEl(nowNode, table.FindRecordId(nowToken->value));						//!!!!!!!
			if (!Next())
				return false;
			if (nowToken->type == TokenType::DELIM) {
				action.push_back(nowToken->value);												//-------------
				//tree.AddExprEl(nowNode, nowToken->value);										//!!!!!!
				if (!Next())
					return false;
				if (!(nowToken->type == TokenType::ID || nowToken->value == "math"))
					return false;
				sqrt += nowToken->value;
				if (nowToken->type == TokenType::ID) {
					table.FindRecordId(nowToken->value);
					//tree.AddExprEl(nowNode, table.FindRecordId(nowToken->value));				//!!!!!
				}
					if (!Next())
					return false;

				if (!(nowToken->type == TokenType::DELIM || nowToken->value == "."))
					return false;
				sqrt += nowToken->value;
				if (nowToken->value != ".") {
					//tree.AddExprEl(nowNode, nowToken->value);									//!!!!!
				}
					if (!Next())
					return false;
				if (!(nowToken->type == TokenType::ID || nowToken->value == "Sqrt"))
					return false;
				sqrt += nowToken->value;
				if (nowToken->value != "Sqrt") {
					table.FindRecordId(nowToken->value);
					//tree.AddExprEl(nowNode, table.FindRecordId(nowToken->value));				//!!!!
				}
				else {
					//tree.AddExprEl(nowNode, "Sqrt");											//!!!!
				}
					action.push_back(sqrt);															//-------------
				if (!Next())
					return false;

				if (!(nowToken->type == TokenType::DELIM || nowToken->value == "("))
					return false;
				//tree.AddExprEl(nowNode, "(");												//!!!!
				
				if (!Next())
					return false;

				if (nowToken->type != TokenType::ID)
					return false;
				//tree.AddExprEl(nowNode, table.FindRecordId(nowToken->value));				//!!!!!
				table.FindRecordId(nowToken->value);
				action.push_back(nowToken->value);											//-------------
				if (!Next())
					return false;

				if (!(nowToken->type == TokenType::DELIM || nowToken->value == ")"))
					return false;
				//tree.AddExprEl(nowNode, ")");												//!!!!
				if (!Next())
					return false;
			}
														//!!!!!!!
			//----------------------------------------------------------------------------------------
			if (yes) {
				cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
				OutAction = tree.startOPZ(action);															//------------
				cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
				for (int i = 0; i < OutAction.size(); i++) {
					if (OutAction[i].size() > 1 && OutAction[i] != "math.Sqrt") {
						tree.AddExprEl(nowNode, table.FindRecordId(OutAction[i]));
					}
					else {
						if (OutAction[i] == "math.Sqrt")
							tree.AddExprEl(nowNode, OutAction.at(i));
						if ((OutAction.at(i)[0] >= 'a' && OutAction.at(i)[0] <= 'z') || (OutAction.at(i)[0] >= 'A' && OutAction.at(i)[0] <= 'Z')) {
							tree.AddExprEl(nowNode, table.FindRecordId(OutAction[i]));
						}
						else {
							tree.AddExprEl(nowNode, OutAction.at(i));
						}
					}
				}

			}
			//----------------------------------------------------------------------------------------		   
			cout << "found sqrt" << endl;
			if (tree.ToParent(nowNode))														//!!!!!!!
				nowNode = tree.ToParent(nowNode);
			yes = false;
			Save();
			if (!Action())
				Load();
			return true;
		}
		bool IDorNum = false;
		int num = 0;
		if (!(nowToken->type == TokenType::NUM || nowToken->type == TokenType::ID))
			return false;
		if (nowToken->type == TokenType::ID) {
			IDorNum = true;
			num = table.FindRecordId(nowToken->value);
			//tree.AddExprEl(nowNode, table.FindRecordId(nowToken->value));						//!!!!!!
			action.push_back(nowToken->value);													//-------------
		}
		if (nowToken->type == TokenType::NUM) {
			IDorNum = false;
			string temp = nowToken->value;
			//tree.AddExprEl(nowNode, nowToken->value);											//!!!!!!
			action.push_back(nowToken->value);													//-------------
			if (Next())
				if (nowToken->type == TokenType::NEWLINE) {
					//table.FindValueToName(name, temp);	//это я совсем закомментировала, не для конечного результата
					if (Prev()) {};
				}
		}
		if (!Next())
			return false;


		Save();
		if (!EvenAction())
			Load();

		if (nowToken->value == "[") {
			if (!Next())
				return false;
			if (nowToken->type != TokenType::ID)
				return false;
			if (!Next())
				return false;
			Save();
			if (!EvenAction())
				Load();
			if (nowToken->value != "]")
				return false;
			if (!Next())
				return false;
			cout << "found action with array" << endl;
		}

		if (yes) {
			cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
			OutAction = tree.startOPZ(action);															//------------
			cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
			for (int i = 0; i < OutAction.size(); i++) {
				if (OutAction[i].size() > 1 && OutAction[i] != "math.Sqrt") {
					tree.AddExprEl(nowNode, table.FindRecordId(OutAction[i]));
				}else
				if ((OutAction.at(i)[0] >= 'a' && OutAction.at(i)[0] <= 'z') || (OutAction.at(i)[0] >= 'A' && OutAction.at(i)[0] <= 'Z')) {
					tree.AddExprEl(nowNode, table.FindRecordId(OutAction[i]));
				}
				else {
					tree.AddExprEl(nowNode, OutAction.at(i));
				}
			}

		}
		else {
			if (IDorNum) {
				tree.AddExprEl(nowNode, num);
			}
			else {
				tree.AddExprEl(nowNode, action[0]);
			}
		}

		if (tree.ToParent(nowNode))															//!!!!!!!
			nowNode = tree.ToParent(nowNode);												//!!!!!!!
		
		cout << "found action" << endl;

		Save();
		if (!Action())
			Load();
		
		
		action.clear();																		//------------
		yes = false;
		if (nowToken->type == TokenType::NEWLINE) {
			if (!Next())
				return false;
		}

		return true;
	}
	bool EvenAction() {
		if (nowToken->type != TokenType::DELIM || nowToken->value == "[")
			return false;
		//tree.AddExprEl(nowNode, nowToken->value);												//!!!!!!!
		action.push_back(nowToken->value);														//----------
		if (!Next())
			return false;

		if (!(nowToken->type == TokenType::NUM || nowToken->type == TokenType::ID))
			return false;
		if (nowToken->type == TokenType::ID) {
			table.FindRecordId(nowToken->value);
			//tree.AddExprEl(nowNode, table.FindRecordId(nowToken->value));					//!!!!!!!!!
			action.push_back(nowToken->value);												//-----------
		}
		else {
			//tree.AddExprEl(nowNode, nowToken->value);										//!!!!!!!!!
			action.push_back(nowToken->value);												//-----------
		}
		if (!Next())
			return false;
		yes = true;
		Save();
		if (!EvenAction())
			Load();

		return true;
	}
	bool ConditionIF() {
		while (nowToken->type == TokenType::NEWLINE)
			nowToken++;

		if (!(nowToken->type == TokenType::KEYWORDS && nowToken->value == "if"))
			return false;
		nowNode = tree.AddChild(nowNode, "condition IF");									//!!!!!!!!!!
		if (!Next())
			return false;

		cout << "Start condition IF" << endl;

		if (!(nowToken->type == TokenType::ID || nowToken->type == TokenType::NUM))
			return false;
		nowNode = tree.AddChild(nowNode, "id", table.FindRecordId(nowToken->value));		//!!!!!!!!!!!
		if (!Next())
			return false;

		if (!(nowToken->type == TokenType::DELIM || nowToken->type == TokenType::LOG))
			return false;
		tree.AddExprEl(nowNode, nowToken->value);											//!!!!!!!!!!!
		if (!Next())
			return false;

		if (!(nowToken->type == TokenType::ID || nowToken->type == TokenType::NUM))
			return false;
		tree.AddExprEl(nowNode, nowToken->value);											//!!!!!!!!!!!
		if (!Next())
			return false;
		
		if (!OpenBracket())
			return false;

		nowToken++;
		if (nowToken->value == "(") {
			nowToken--;
			if (!CauseFunc())
				return false;
			if (!Next())
				return false;

			if (!CloseBracket())
				return false;

			while (nowToken->type == TokenType::NEWLINE)
				nowToken++;
			nowNode = tree.ToParent(nowNode);
			nowNode = tree.ToParent(nowNode);
			Save();
			if (!ConditionIF())
				Load();
			
			return true;
		}
		
		nowToken--;
		if (!Action())
			return false;
		if (Print())
			cout << "";

		if (!CloseBracket())
			return false;

		if (nowToken->type == TokenType::NEWLINE) {
			nowToken++;
			return true;
		}

		if (nowToken->value == "}") {
			nowNode = tree.ToParent(nowNode);
			nowNode = tree.ToParent(nowNode);
			return true;
		}

		if (!(nowToken->type == TokenType::KEYWORDS || nowToken->value == "else")) {
			return false;
		}
		nowNode = tree.AddChild(nowNode, "else");
		if (!Next())
			return false;
		cout << "Finish condition IF" << endl;
		if (nowToken->value == "if") {
			if (!ConditionIF())
				return false;
		}
		else {
			if (!OpenBracket())
				return false;

			if (!Print())
				return false;

			if (!CloseBracket())
				return false;
		}
		
		return true;
	}
	bool ConditionFOR() {
		while (nowToken->type == TokenType::NEWLINE)
			nowToken++;

		if (!(nowToken->type == TokenType::KEYWORDS && nowToken->value == "for"))
			return false;
		if (!Next())
			return false;

		cout << "Start condition FOR" << endl;

		if (nowToken->type != TokenType::ID)
			return false;
		string name = nowToken->value;
		nowNode = tree.AddChild(nowNode, "condition FOR", table.FindRecordId(nowToken->value));
		if (!Next())
			return false;

		if (!(nowToken->type == TokenType::DELIM || nowToken->value == "="))
			return false;
		if (!Next())
			return false;

		if (!(nowToken->type == TokenType::ID || nowToken->type == TokenType::NUM))
			return false;
		string min = nowToken->value;
		if (!Next())
			return false;

		if (!(nowToken->type == TokenType::DELIM || nowToken->value == ";"))
			return false;
		if (!Next())
			return false;

		if (nowToken->type != TokenType::ID)
			return false;
		if (!Next())
			return false;

		if (!(nowToken->type == TokenType::DELIM || nowToken->type == TokenType::LOG))
			return false;
		if (!Next())
			return false;

		if (!(nowToken->type == TokenType::ID || nowToken->type == TokenType::NUM))
			return false;
		string max = nowToken->value;
		if (!Next())
			return false;

		if (!(nowToken->type == TokenType::DELIM || nowToken->value == ";"))
			return false;
		if (!Next())
			return false;

		if (nowToken->type != TokenType::ID)
			return false;
		if (!Next())
			return false;

		if (nowToken->type != TokenType::MATH)
			return false;
		string step = "1";
		if (!Next())
			return false;

		table.FindValueInFOR(name, min, max, step);

		if (!OpenBracket())
			return false;

		if (!(Action() || Print()))
			return false;

		if (!CloseBracket())
			return false;

		if (nowToken->type == TokenType::NEWLINE) {
			if (!Next())
				return false;
		}
		nowNode = tree.ToParent(nowNode);
		cout << "Finish condition FOR" << endl;
		return true;
	}
	bool CauseFunc() {
		while (nowToken->type == TokenType::NEWLINE)
			nowToken++;

		if (nowToken->type != TokenType::ID)
			return false;
		nowNode = tree.AddChild(nowNode, "function call", nowToken->value);
		if (!Next())
			return false;

		if (!(nowToken->type == TokenType::DELIM && nowToken->value == "("))
			return false;
		if (!Next())
			return false;

		if (!(nowToken->type == TokenType::ID || nowToken->type == TokenType::NUM))
			return false;
		tree.AddChild(nowNode, "id", table.FindRecordId(nowToken->value));				//!!!!!!!!!!
		
		if (!Next())
			return false;

		Save();
		if (!ParamFunc())
			Load();

		if (!(nowToken->type == TokenType::DELIM && nowToken->value == ")"))
			return false;
		if (!Next())
			return false;
		nowNode = tree.ToParent(nowNode);
		cout << "found cause func" << endl;
		return true;
	}
	bool ParamFunc() {
		if (!(nowToken->type == TokenType::DELIM || nowToken->value == ","))
			return false;
		if (!Next())
			return false;

		if (!(nowToken->type == TokenType::NUM || nowToken->type == TokenType::ID))
			return false;
		if (nowToken->type == TokenType::NUM)
			tree.AddChild(nowNode, "data", nowToken->value);								//!!!!!!!!!!
		else 
			tree.AddChild(nowNode, "id", table.FindRecordId(nowToken->value));				//!!!!!!!!!!
		if (!Next())
			return false;

		Save();
		if (!ParamFunc())
			Load();

		return true;
	}

	bool OpenBracket() {
		if (nowToken->type != TokenType::DELIM || nowToken->value != "{")
			return false;
		else first++;
		if (!Next())
			return false;
		if (nowToken->type != TokenType::NEWLINE)
			return false;
		if (!Next())
			return false;
		cout << "Found {" << endl;
		return true;
	}
	bool CloseBracket() {

		if (!(nowToken->type == TokenType::DELIM || nowToken->value == "}"))
			return false; else second++;
		if (!Next())
			return false;

		if (nowToken->type == TokenType::NEWLINE) {
			if (!Next())
				return false;
			cout << "found }" << endl;
			return true;
		}
	}
	bool CloseBracketEnd() {
		while (nowToken->type == TokenType::NEWLINE)
			nowToken++;

		if (!(nowToken->type == TokenType::DELIM || nowToken->value == "}"))
			return false; else second++;
		if (!Next())
			return false;

		if (nowToken->type == TokenType::NEWLINE) {
			cout << "found }" << endl;
			if (first != second)
				cout << "Error: count { and } not same" << endl;
			return true;
		}
		return true;
	}
	bool Print() {
		while (nowToken->type == TokenType::NEWLINE)
			nowToken++;

		if (!(nowToken->type == TokenType::ID || nowToken->value == "fmt"))
			return false;
		string typeVar = nowToken->value;
		if (!Next())
			return false;

		if (!(nowToken->type == TokenType::DELIM || nowToken->value == "."))
			return false;
		if (!Next())
			return false;

		if (!(nowToken->type == TokenType::ID || nowToken->value == "Print"))
			return false;
		string kind = nowToken->value;
		if (!Next())
			return false;

		if (!(nowToken->type == TokenType::DELIM || nowToken->value == "("))
			return false;
		if (!Next())
			return false;

		if (nowToken->type != TokenType::STRING)
			if (nowToken->type == TokenType::DELIM)
				return false;
		string value = nowToken->value;
		if (!Next())
			return false;

		if (!(nowToken->value == ")")) {
			if (!(nowToken->type == TokenType::DELIM || nowToken->value == ","))
				return false;
			if (!Next())
				return false;

			if (nowToken->type != TokenType::ID)
				return false;
			value += nowToken->value;
			if (!Next())
				return false;
		}

		if (!(nowToken->type == TokenType::DELIM || nowToken->value == ")"))
			return false;
		if (!Next())
			return false;


		if (nowToken->type == TokenType::NEWLINE) {
			if (!Next())
				return false;
			cout << "found Print" << endl;
		}
		nowNode = tree.AddChild(nowNode, "Print", table.AddVar("Output", kind, typeVar, value));
		nowNode = tree.ToParent(nowNode);
		
		Save();
		if (!Print())
			Load();

		return true;
	}

	bool FuncDefinition() {
		while (nowToken->type == TokenType::NEWLINE)
			nowToken++;
		nowNode = tree.ToRoot();
		if (nowToken->type != TokenType::KEYWORDS || nowToken->value != "func")
			return false;
		string kind = nowToken->value;
		if (!Next())
			return false;

		if (nowToken->type != TokenType::ID)
			return false;
		string name = nowToken->value;
		if (!Next())
			return false;

		if (!(nowToken->type == TokenType::DELIM && nowToken->value == "("))
			return false;
		if (!Next())
			return false;

		if (nowToken->type != TokenType::ID)
			return false;
		Param p;
		p.NameParam = nowToken->value;
		if (!Next())
			return false;

		if (!(nowToken->type == TokenType::DELIM && nowToken->value == "["))
			return false;
		string typeParam = nowToken->value;
		if (!Next())
			return false;

		if (!(nowToken->type == TokenType::DELIM && nowToken->value == "]"))
			return false;
		typeParam += nowToken->value;
		if (!Next())
			return false;

		if (nowToken->type != TokenType::KEYWORDS)
			return false;
		typeParam += nowToken->value;
		p.TypeParam = typeParam;
		if (!Next())
			return false;
		param.push_back(p);
		if (!(nowToken->type == TokenType::DELIM || nowToken->value == ","))
			return false;
		if (!Next())
			return false;

		if (nowToken->type != TokenType::ID)
			return false;
		p.NameParam = nowToken->value;
		if (!Next())
			return false;

		if (nowToken->type != TokenType::KEYWORDS)
			return false;
		p.TypeParam = nowToken->value;
		param.push_back(p);
		if (!Next())
			return false;

		if (!(nowToken->type == TokenType::DELIM || nowToken->value == ","))
			return false;
		if (!Next())
			return false;

		if (nowToken->type != TokenType::ID)
			return false;
		p.NameParam = nowToken->value;
		if (!Next())
			return false;

		if (nowToken->type != TokenType::KEYWORDS)
			return false;
		p.TypeParam = nowToken->value;
		param.push_back(p);
		if (!Next())
			return false;
		table.AddFunc(name, kind, param);
		string str = "func " + name;
		nowNode = tree.AddChild(nowNode, str);															//!!!!!
		cout << "------------------" << endl;
		for (int i = 0; i < param.size(); i++){
			cout << "Name = " << param[i].NameParam << "	" << "Type = " << param[i].TypeParam << endl;
			table.AddVar(param[i].NameParam, "param", param[i].TypeParam, "None");
		}
		cout << "---------------------" << endl;

		if (!(nowToken->type == TokenType::DELIM && nowToken->value == ")"))
			return false;
		if (!Next())
			return false;

		
		cout << "found new func" << endl;

		if (!OpenBracket())
			return false;

		if (!FuncVar())
			return false;

		if (!Action())
			return false;

		if (!ConditionWHILE())
			return false;

		if (!ConditionIF())
			return false;

		if (!CloseBracketEnd())
			return false;

		return true;
	}

	bool ConditionWHILE() {
		while (nowToken->type == TokenType::NEWLINE)
			nowToken++;

		cout << "Start condition WHILE" << endl;
		nowNode = tree.AddChild(nowNode, "condition WHILE");

		if (!(nowToken->type == TokenType::KEYWORDS && nowToken->value == "for"))
			return false;
		if (!Next())
			return false;

		if (nowToken->type != TokenType::ID)
			return false;
		nowNode = tree.AddChild(nowNode, "id", table.FindRecordId(nowToken->value));		//!!!!!!!!!!!
		if (!Next())
			return false;

		if (!(nowToken->type == TokenType::DELIM || nowToken->type == TokenType::LOG))
			return false;
		tree.AddExprEl(nowNode, nowToken->value);											//!!!!!!!!!!!
		if (!Next())
			return false;

		if (!(nowToken->type == TokenType::ID || nowToken->type == TokenType::NUM))
			return false;
		tree.AddExprEl(nowNode, table.FindRecordId(nowToken->value));						//!!!!!!!!!!!
		if (!Next())
			return false;

		if (!OpenBracket())
			return false;

		if (!EvenConditionWHILE())
			return false;

		if (!ConditionIF())
			return false;

		if (!CloseBracket())
			return false;

		nowNode = tree.ToParent(nowNode);
		nowNode = tree.ToParent(nowNode);
		cout << "Finish condition WHILE" << endl;

		return true;
	}
	bool EvenConditionWHILE() {
		while (nowToken->type == TokenType::NEWLINE)
			nowToken++;
		if (temp) nowNode = tree.ToParent(nowNode);
		temp++;
		if (!(nowToken->type == TokenType::KEYWORDS && nowToken->value == "for"))
			return false;
		cout << "Start condition WHILE" << endl;
		nowNode = tree.AddChild(nowNode, "condition WHILE");								//!!!!!!!!!!
		if (!Next())
			return false;

		if (nowToken->type != TokenType::ID)
			return false;
		nowNode = tree.AddChild(nowNode, "id", table.FindRecordId(nowToken->value));		//!!!!!!!!!!!
		if (!Next())
			return false;

		if (!(nowToken->type == TokenType::DELIM || nowToken->value == "["))
			return false;
		tree.AddExprEl(nowNode, nowToken->value);											//!!!!!!!!!!!
		if (!Next())
			return false;

		if (nowToken->type != TokenType::ID)
			return false;
		tree.AddExprEl(nowNode, table.FindRecordId(nowToken->value));						//!!!!!!!!!!!
		if (!Next())
			return false;

		if (!(nowToken->type == TokenType::DELIM || nowToken->value == "]"))
			return false;
		tree.AddExprEl(nowNode, nowToken->value);											//!!!!!!!!!!!
		if (!Next())
			return false;

		if (!(nowToken->type == TokenType::DELIM || nowToken->type == TokenType::LOG))
			return false;
		tree.AddExprEl(nowNode, nowToken->value);											//!!!!!!!!!!!
		if (!Next())
			return false;

		if (nowToken->type != TokenType::ID)
			return false;
		tree.AddExprEl(nowNode, table.FindRecordId(nowToken->value));						//!!!!!!!!!!!
		if (!Next())
			return false;

		if (!OpenBracket())
			return false;

		if (!Action())
			return false;

		if (!CloseBracket())
			return false;
		
		nowNode = tree.ToParent(nowNode);

		Save();
		if (!EvenConditionWHILE())
			Load();

		return true;
	}

public:
	Syntaxer() {}
	bool Parse(vector <Token>& tokens) {
		this->tokens = &tokens;
		nowToken = this->tokens->begin();
		if (!this->tokens)
			return false;
		if (!Init()) {
			cout << "Error in token:" << nowToken->value << endl;
			return false;
		}
		tree.Print();
		gen.Generate(tree, table);
		return true;
	}
};
#endif