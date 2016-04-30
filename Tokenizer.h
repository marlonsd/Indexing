#ifndef TOKENIZER_H
#define TOKENIZER_H


#include <iostream>
#include <vector>
#include <string>
#include "func.h"

using namespace std;

class Tokenizer{
private:
	vector<string> tokens;

	void normalizeWord(string& s);
	void split(const string& s, char c, vector<string>& v);
	void generatingTokens(const string& s, vector<string>& v);

public:
	Tokenizer();
	Tokenizer(const string& s);

	void addTokens(const string& s);
	void addTokens(const vector<string>& v);

	string getToken();

	int size();
	bool empty();
};

#endif