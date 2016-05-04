#include <iostream>
#include <unordered_map>
#include <html/ParserDom.h>
#include "Tokenizer.h"

#define FILENAME "htmls"

struct FileList {
	int file_index;
	vector<int> position;

	bool operator==(const FileList &other) const
	{ return (file_index == other.file_index
			&& position == other.position);
	}
};

void indexing(string doc, int index, string url, unordered_map<string, vector<FileList>>& inverted_index);

int main(int argc, const char * argv[]) {  

	fstream input;
	string acc, url;
	int pipe_count = 0;
	int file_index = 0;

	unordered_map<string, vector<FileList>> inverted_index;

	input.open(FILENAME, ios::in);

	if (input.is_open()){
		while (!input.eof()){
			string aux;
			input >> aux;

			// cout << aux << endl;

			std::size_t found = aux.find("|||");

			if (found != std::string::npos) {
				pipe_count++;
				if (pipe_count >= 3){
					
					indexing(acc, file_index, url, inverted_index);
					
					pipe_count = 1;
					file_index++;

					acc = "";
					url = "";
				}
			} else {

				if (pipe_count <= 1){
					url+=aux+" ";
				} else {
					acc+=aux+" ";
				}
			}
		}
	}

	for (auto i : inverted_index){
		vector<FileList> list_of_files = i.second;
		cout << i.first << endl;
		for (auto v : list_of_files){
			cout << "Document: " << v.file_index << endl;
			for (auto val : v.position){
				cout << val << " ";
			}
			cout << endl;
		}
		cout << endl;
		cout << endl;
	}

	exit(0);
}

void indexing(string doc, int index, string url, unordered_map<string, vector<FileList>>& inverted_index){
	cout << "\t\tDocument " << index << endl;
	cout << "URL:\t " << url << endl;
	cout << "Content: " << doc << endl;
	cout << "------------------" << endl << endl;	

	//Parse some html code
	// string html = "<html><body>hey, this Is (é) the whole vector<char> v</body></html>";
	htmlcxx::HTML::ParserDom parser;
	tree<htmlcxx::HTML::Node> dom = parser.parseTree(doc);

	//Print whole DOM tree
	cout << dom << std::endl;

	//Dump all links in the tree
	tree<htmlcxx::HTML::Node>::iterator it = dom.begin();
	tree<htmlcxx::HTML::Node>::iterator end = dom.end(); 

	// for (; it != end; ++it) {
	// 	cout << it->tagName() << endl;
	// 	if (it->tagName() == "A") {
	// 		it->parseAttributes();
	// 		std::cout << it->attributes("href");
	// 	}
	// }

	//Dump all text of the document
	it = dom.begin();
	end = dom.end();
	string text = "";

	for (; it != end; ++it) {
		if ((!it->isTag()) && (!it->isComment())) {
			// std::cout << it->text() << endl;
			text.append(it->text());
		}
	}

	// cout << text << endl;
	Tokenizer t(text);

	cout << "Tokenized words" << endl;
	int word_id = 0;
	while (t.size() > 0){
		string token = t.getToken();

		if (token.size()) {

			auto search = inverted_index.find(token);

			if(search != inverted_index.end()) {
				// Token is in index already

				// Is There an entry for the document?
				if (inverted_index[token].back().file_index == index){
					inverted_index[token].back().position.push_back(word_id);					
				}
				else {
					FileList list;
					list.file_index = index;
					list.position.push_back(word_id);

					inverted_index[token].push_back(list);
				}
			}
			else {
				// Token has not been added to the index yet
				FileList list;
				list.file_index = index;
				list.position.push_back(word_id);

				cout << token << " " << index << " " << word_id << endl;

				inverted_index.insert(std::make_pair(token,vector<FileList>()));

				inverted_index[token].push_back(list);

			}
			word_id++;
		}
	}
}