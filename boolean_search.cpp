#include "Inverted_Index.h"
#include "func.h"			// Defines are here

int main(int argc, const char* argv[]) {  

	string query;
	std::size_t found;
	InvertedIndex index;
	Tokenizer tokens;

	cout << "Query: ";
	getline(cin, query);

	tokens.addTokens(query, load_stop_words(STOPWORDS_PATH));

	cout << endl;

	cout << "Processing..." << endl;
	while(tokens.size()){
		cout << tokens.getToken() << endl;
	}

	query = "";

	rewind(stdin);
	getline(cin, query);

	cout << query;

	exit(0);
}