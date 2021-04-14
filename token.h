#ifndef TOKEN_H
#define TOKEN_H
#include <iostream>
#include <string>
using namespace std;

enum TokenType {				//множество всех типов токена
	NUM,
	ID,
	DELIM,
	KEYWORDS,
	LOG,
	MATH,
	STRING,
	NEWLINE
};

struct Token {
	TokenType type;			//тип токена
	string value;			//его значение
};
#endif