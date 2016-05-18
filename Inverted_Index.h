#ifndef INVERTED_INDEX_H
#define INVERTED_INDEX_H

#include "func.h"
#include "Tokenizer.h"

using namespace std;

class InvertedIndex{
private:
	string vocabulary_buffer;
	int memory_usage;
	int total_size_index;
	int word_index;
	int total_token;

	unordered_map<string, int> vocabulary;					// <word, id>
	unordered_map<string,vector<FileList>> inverted_index;	// <id_word, list of occurrences>

	void memory_dump();
	vector<string> get_vocabulary();
	void load_vocabulary();
	void load_full_index();

public:
	InvertedIndex();
	InvertedIndex(Tokenizer& t, int index = 0);

	void indexing(Tokenizer& t, int index = 0);
	void sorted_index();
	void vocabulary_dump();
	void load_index();
	vector<FileList> get_list(string& token);

};

#endif