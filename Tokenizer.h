#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <deque>
#include "func.h"


using namespace std;

class Tokenizer{
private:
	deque<string> tokens;

	void normalizeWord(string& s);
	void generatingTokens(const string& s, deque<string>& v);
	void generatingTokens(const string& s, deque<string>& v, const unordered_set<string>& stopwords);

public:
	Tokenizer();
	Tokenizer(const string& s);
	Tokenizer(const string& s, const unordered_set<string>& stopwords);

	void addTokens(const string& s);
	void addTokens(const deque<string>& v);
	void addTokens(const string& s, const unordered_set<string>& stopwords);
	void addTokens(const deque<string>& v, const unordered_set<string>& stopwords);

	string getToken();

	int size();
	bool empty();
};

#endif