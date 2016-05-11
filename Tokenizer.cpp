#include "Tokenizer.h"

Tokenizer::Tokenizer(){
	
}

Tokenizer::Tokenizer(const string& s){
	this->generatingTokens(s, this->tokens);
	// cout << s << this->tokens.size();
}

// Need to find out how tokens must be, especially their ending
void Tokenizer::normalizeWord(string& s){
	unsigned int i;

	// Removing accents, punctuation and converting to lower case
	cleaningWord(s);

	// Removing whitespaces within the token
	s.erase(std::remove_if(s.begin(), s.end(),[](char x){return std::isspace(x);}),s.end());

}

// Splits a string "s" in "c"
void Tokenizer::split(const string& s, char c, vector<string>& v) {
	string::size_type i = 0;
	string::size_type j = s.find(c);

	if (j == string::npos) {
		v.push_back(s);
	} else {
		while (j != string::npos) {
			v.push_back(s.substr(i, j-i));
			i = ++j;
			j = s.find(c, j);

			if (j == string::npos)
			v.push_back(s.substr(i, s.length()));
		}
	}
}

void Tokenizer::generatingTokens(const string& s, vector<string>& v){
	vector<string> lines, aux;

	this->split(s, '\n', lines);

	for (string line : lines){
		this->split(s, ' ', aux);

		for (string e : aux){

			if (aux.size()){
				this->normalizeWord(e);
				if (e.size()){
					v.push_back(e);
				}
			}

		}
		aux.clear();
	}
	lines.clear();
}

void Tokenizer::addTokens(const string& s){
	vector<string> aux;

	this->generatingTokens(s, aux);

	for (string e : aux){
		if (e.size()){
			this->tokens.push_back(e);
		}
	}

	aux.clear();
}

void Tokenizer::addTokens(const vector<string>& v){
	for (string e : v){
		if (e.size()){
			this->tokens.push_back(e);
		}
	}
}

string Tokenizer::getToken(){
	string token = "";

	if (this->tokens.size()){
		token = this->tokens.front();
		this->tokens.erase(this->tokens.cbegin());
	}

	return token;
}

int Tokenizer::size(){
	return this->tokens.size();
}

bool Tokenizer::empty(){
	return this->tokens.empty();
}