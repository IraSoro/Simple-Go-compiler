#include "lexer.h"
#include "syntaxer.h"


int main(){
	string filepath="prog2.go";
	vector<Token> tokens;
	Lexer lex;
	if(!lex.GetTokens(filepath, tokens)){
		return -1;
	}
	vector<string> types = {"num", "id", "delim", "keywords", "log", "math", "string", "new_line"};
	for(unsigned int i=0; i<tokens.size(); i++){
		cout<< "(" << types[tokens[i].type] << ")" << tokens[i].value <<endl;
	}
	Syntaxer synt;
	synt.Parse(tokens);

	return 0;
}