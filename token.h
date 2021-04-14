#ifndef TOKEN_H
#define TOKEN_H
#include <iostream>
#include <string>
using namespace std;

enum TokenType {				//��������� ���� ����� ������
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
	TokenType type;			//��� ������
	string value;			//��� ��������
};
#endif