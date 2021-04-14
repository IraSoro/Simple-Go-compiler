#ifndef LEXER_H
#define LEXER_H
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "Bor.h"
#include "token.h"
#include <map>

using namespace std;

class Lexer {
private:
	enum State {				//состояния конечного автомата
		INIT,
		NUM,
		ID
	};
	enum Output {			//выходные сигналы
		O_NUM,
		O_ID,
		O_DELIM,
		O_KEYWORD,
		O_LOG,
		O_MATH,
		O_STRING,
		O_NEWLINE,
		O_IGNORE,
		O_NONE
	};
	using Input = pair<State, char>;		//текущее состояние, входной символ
	using Result = pair<State, Output>;		//следующее состояние, выходной сигнал
	using Element = pair<Input, Result>;	//элемент конечного автомата
	map<Input, Result> fa;

	State nowState = State::INIT;			//текущее состояние конечного автомата
	string delim = " \n\t+-;,-*%/.(){}[]\"'<>=";

	//добавление перехода в автомат																																			//принимает состояние, из которого происходит переход, символ, по которому он происходит, следующее состояние и выходной сигнал
	void AddInAutomat(State from, char sym, State to, Output output) {
		fa.insert(Element(Input(from, sym), Result(to, output)));
	}

	void Init() {
		//инициализация операторов
		for (unsigned int i = 0; i < delim.size(); i++)
			AddInAutomat(State::INIT, delim[i], State::INIT, Output::O_DELIM);

		//AddInAutomat(State::INIT, '\n', State::INIT, Output::O_NEWLINE);

		//чисел
		for (unsigned char sym = '0'; sym <= '9'; sym++) {
			AddInAutomat(State::INIT, sym, State::NUM, Output::O_NONE);
			AddInAutomat(State::NUM, sym, State::NUM, Output::O_NONE);
			AddInAutomat(State::ID, sym, State::ID, Output::O_NONE);
		}

		//идентификаторов
		for (unsigned char sym = 'a'; sym <= 'z'; sym++) {
			AddInAutomat(State::INIT, sym, State::ID, Output::O_NONE);
			AddInAutomat(State::ID, sym, State::ID, Output::O_NONE);
		}
		for (unsigned char sym = 'A'; sym <= 'Z'; sym++) {
			AddInAutomat(State::INIT, sym, State::ID, Output::O_NONE);
			AddInAutomat(State::ID, sym, State::ID, Output::O_NONE);
		}
		AddInAutomat(State::ID, '_', State::ID, Output::O_NONE);

		//добавление переходов в конечные состояния для чисел и идентификаторов
		for (unsigned int i = 0; i < delim.size(); i++) {
			AddInAutomat(State::ID, delim[i], State::INIT, Output::O_ID);
			AddInAutomat(State::NUM, delim[i], State::INIT, Output::O_NUM);
		}
	}
public:

	bool GetTokens(string filepath, vector<Token>& tokens) {
		Init();
		ifstream file(filepath);
		string line;
		unsigned int lineCnt = 0;
		while (getline(file, line)) {
			if (IsEmty(line))
				continue;
			line = FoundforComments(line);
			line += '\n';
			lineCnt++;
			string tokenValue = "";
			for (unsigned int i = 0; i < line.size(); i++) {
				map<Input, Result>::iterator el = fa.find(Input(nowState, line[i]));	//шаблонный класс				//ищем переход для текущего состояния и входного символа
				if (el == fa.end()) {//если не нашли, выдаем ошибку
					line.insert(line.begin() + i + 1, '<'); line.insert(line.begin() + i + 2, '-'); line.insert(line.begin() + i + 3, '-');
					cout << "[" << lineCnt << "]Lexer error:Wrong symbol: " << line << endl;
					return false;
				}
				tokenValue += line[i];
				if (line[i] == '+' && line[i - 1] == '+') {
					tokenValue = "";
					continue;
				}
				if (line[i] == '-' && line[i - 1] == '-') {
					tokenValue = "";
					continue;
				}
				if (line[i] == '=' && (line[i - 1] == '>' || line[i - 1] == '<' || line[i - 1] == '=')) {
					tokenValue = "";
					continue;
				}

				Result res = el->second;
				nowState = res.first;			//переходим в следующее состояние
				Output outSignal = res.second;	//проверяем выходной сигнал

				if (outSignal == Output::O_NONE)	//если токен пока неизвестен, продолжаем чтение
					continue;


				if (outSignal == Output::O_IGNORE) {
					tokenValue = "";
					continue;
				}

				if (IsEmty(tokenValue)) {
					tokenValue = "";
					continue;
				}
				if (outSignal <= Output::O_ID) {	//если токен ID или NUM, значит мы захватили лишний символ
					i--;						//возвращаем его обратно
					tokenValue = tokenValue.substr(0, tokenValue.size() - 1);	//удаляем из значения токена

				}
				if (outSignal == Output::O_ID && fount_in_bor(tokenValue))
					outSignal = O_KEYWORD;

				if (line[i] == '+' && line[i + 1] == '+') {
					outSignal = O_MATH;
					tokenValue += "+";
				}

				if (line[i] == '-' && line[i + 1] == '-') {
					outSignal = O_MATH;
					tokenValue += "-";
				}

				if ((line[i] == '>' || line[i] == '<' || line[i] == '=') && line[i + 1] == '=') {
					outSignal = O_LOG;
					tokenValue += "=";
				}

				if (line[i] == '"') {
					string str = FoundString(line);
					line = DeliteString(line);
					tokenValue = str;
					outSignal = O_STRING;
				}

				tokens.push_back(Token{ (TokenType)outSignal, tokenValue });	//добавляем токен
				tokenValue = "";
			}
			tokens.push_back(Token{ (TokenType)NEWLINE, "" });
		}
		return true;
	}

	string DeliteString(string str) {
		int x1 = 0, x2 = 0;
		string str2;
		for (int i = 0; i < str.size(); i++) {
			if (str[i] == '"') {
				x1 = i;
				break;
			}
			else str2 += str[i];
		}

		for (int i = x1 + 1; i < str.size(); i++) {
			if (str[i] == '"') {
				x2 = i;
				break;
			}
		}

		for (int i = x2 + 1; i < str.size(); i++) {
			str2 += str[i];
		}

		return str2;
	}

	string FoundString(string str) {
		string str2;
		int x = 0;
		for (int i = 0; i < str.size(); i++)
			if (str[i] == '"') {
				x = i;
				break;
			}
		str2 += str[x];
		x++;
		for (int i = x; i < str.size(); i++) {
			if (str[i] != '"')
				str2 += str[i]; else break;
		}
		str2 += '"';
		return str2;
	}

	string FoundforComments(string str1) {
		string str2;
		int len = str1.size();
		for (int i = 1; i < len; i++)
			if (str1[i] == '/' && str1[i - 1] == '/') {
				str2 = str1.substr(0, i - 1);
				return str2;
			}
		return str1;
	}

	bool IsEmty(string str) {
		for (int i = 0; i < str.size(); i++) {
			if (!(str[i] <= ' '))
				return false;
		}
		return true;
	}

	bool fount_in_bor(string tokenValue) {
		BOR bor;
		char key[100];
		ifstream f1("1.txt");
		if (!f1.is_open()) return 0;
		while (!f1.eof()) {
			f1 >> key;

			bor.AddNode(key);
		}
		f1.close();
		if (bor.FindNode((char*)tokenValue.c_str())) return true; else return false;
	}

};
#endif
