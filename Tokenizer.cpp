#include "Tokenizer.h"

Tokenizer::Tokenizer(){
	this->tokens = {};
}

Tokenizer::Tokenizer(const string& s){
	this->generatingTokens(s, this->tokens);
}

Tokenizer::Tokenizer(const string& s, const unordered_set<string>& stopwords){
	this->generatingTokens(s, this->tokens, stopwords);
}

// Need to find out how tokens must be, especially their ending
void Tokenizer::normalizeWord(string& s){
	unsigned int i;

	// Removing accents, punctuation and converting to lower case
	cleaningWord(s);

	// Removing whitespaces within the token
	s.erase(std::remove_if(s.begin(), s.end(),[](char x){return std::isspace(x);}),s.end());

}

void Tokenizer::generatingTokens(const string& s, deque<string>& v){
	vector<string> lines, aux;

	split(s, '\n', lines);

	for (string line : lines){
		split(s, ' ', aux);

		for (string e : aux){

			if (e.size() > 1 && e[0] != '&'){
				this->normalizeWord(e);
				if (e.size() > 1 && e.size() < 30){
					v.push_back(e);
				}
			}

		}
		aux.clear();
	}
	lines.clear();
}

void Tokenizer::generatingTokens(const string& s, deque<string>& v, const unordered_set<string>& stopwords){
	vector<string> lines, aux;

	split(s, '\n', lines);

	for (string line : lines){
		split(s, ' ', aux);

		for (string e : aux){

			if (e.size() > 1 && e[0] != '&'){
				this->normalizeWord(e);
				if (e.size() > 1 && e.size() < 30){
					unordered_set<string>::const_iterator got = stopwords.find(e);
					// Testing if e is not a stop word
					if (got == stopwords.end()){
						v.push_back(e);
					}
				} 
			}

		}
		aux.clear();
	}
	lines.clear();
}

void Tokenizer::addTokens(const string& s){
	this->generatingTokens(s, this->tokens);
}

void Tokenizer::addTokens(const string& s, const unordered_set<string>& stopwords){
	this->generatingTokens(s, this->tokens, stopwords);
}

void Tokenizer::addTokens(const deque<string>& v, const unordered_set<string>& stopwords){
	for (string e : v){
		if (e.size() > 1 && e[0] != '&')
			this->normalizeWord(e);
			if (e.size() > 1 && e.size() < 30){
				unordered_set<string>::const_iterator got = stopwords.find(e);
				// Testing if e is not a stop word
				if (got == stopwords.end()){
					this->tokens.push_back(e);
				}
			}
	}
}

string Tokenizer::getToken(){
	string token = "";

	if (this->tokens.size()){
		token = this->tokens.front();
		this->tokens.pop_front();
	}

	return token;
}

int Tokenizer::size(){
	return this->tokens.size();
}

bool Tokenizer::empty(){
	return this->tokens.empty();
}