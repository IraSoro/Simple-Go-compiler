#ifndef BOR_H
#define BOR_H
#include<cstring>
#include<fstream>
#include<iostream>
#define N 1000
using namespace std;

class Node {
public:
	bool flag;
	Node* array[N];
	Node() {
		for (int i = 0; i < N; i++)
			array[i] = NULL;
		flag = false;
	}
	~Node() {
	}
};


class BOR {
private:
	Node* root = nullptr;
	void DeleteNode(Node* pn) {
		if (!pn) return;
		for (int i = 0; i < N; i++)
			DeleteNode(pn->array[i]);
		delete pn;
	}
public:
	~BOR() {
		for (int i = 0; i < N; i++) {
			if (root->array[i]) DeleteNode(root->array[i]);
		}
		delete root;
	}
	void AddNode(char* str) {
		Node* pn;
		if (root == NULL) {
			root = new Node;
		}
		pn = root;
		int i = 0;
		while (str[i] != '\0') {
			if (pn->array[str[i]] == NULL)
				pn->array[str[i]] = new Node;
			pn = pn->array[str[i]];
			i++;
		}
		pn->flag = true;

	}
	bool FindNode(char* str) {
		Node* pn;
		pn = root;
		int i = 0;
		while (str[i] != '\0') {
			if (pn->array[str[i]] == NULL) {
				return false;
			}

			pn = pn->array[str[i]];
			i++;
		}
		return pn->flag;
	}
};
#endif 

