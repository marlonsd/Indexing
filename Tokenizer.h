#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "func.h"

using namespace std;

class Tokenizer{
private:
	vector<string> tokens;

	void normalizeWord(string& s);
	void generatingTokens(const string& s, vector<string>& v);
	void generatingTokens(const string& s, vector<string>& v, const unordered_set<string>& stopwords);

public:
	Tokenizer();
	Tokenizer(const string& s);
	Tokenizer(const string& s, const unordered_set<string>& stopwords);

	void addTokens(const string& s);
	void addTokens(const vector<string>& v);
	void addTokens(const string& s, const unordered_set<string>& stopwords);
	void addTokens(const vector<string>& v, const unordered_set<string>& stopwords);

	string getToken();

	int size();
	bool empty();
};

#endif