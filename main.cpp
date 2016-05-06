#include <iostream>
#include <unordered_map>
#include <cstdlib>
#include <html/ParserDom.h>
#include "Tokenizer.h"

#define FILENAME "htmls"
// #define MEMORY_LIMITE 250000 // (bytes)
#define MEMORY_LIMITE 160 // (bytes)

#define INDEX_FILE_NAME "index/backup_index"

struct FileList {
	int file_index;
	vector<int> position;

	bool operator==(const FileList &other) const {
		return (file_index == other.file_index
				&& position == other.position);
	}
};

unordered_map<string, int> vocabulary; 					// <word, id>
unordered_map<string,vector<FileList>> inverted_index;	// <id_word, list of occurrences>

int word_index = 0;

int memory_usage = 0;

void indexing(string doc, int index, string url);
void memory_check(bool force=false);

int main(int argc, const char * argv[]) {  

	fstream input;
	string acc, url;
	int pipe_count = 0;
	int file_index = 0;

	input.open(INDEX_FILE_NAME, ios::out);
	input.close();

	input.open(FILENAME, ios::in);

	if (input.is_open()){
		while (!input.eof()){
			string aux;
			input >> aux;

			std::size_t found = aux.find("|||");

			if (found != std::string::npos) {
				pipe_count++;
				if (pipe_count >= 3){
					
					indexing(acc, file_index, url);
					
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

	// for (auto i : inverted_index){
	// 	vector<FileList> list_of_files = i.second;
	// 	cout << i.first << endl;
	// 	for (auto v : list_of_files){
	// 		cout << "Document: " << v.file_index << endl;
	// 		for (auto val : v.position){
	// 			cout << val << " ";
	// 		}
	// 		cout << endl;
	// 	}
	// 	cout << endl;
	// 	cout << endl;
	// }

	for (auto word : vocabulary){
		cout << "Word: \"" << word.first << "\"" << endl;

		for (auto document : inverted_index[word.first]){
			cout << "  Document " << document.file_index << endl;
			cout << "\t";
			for (auto file : document.position){
				cout << file << " ";
			}
			cout << endl;
		}
		cout << endl;
	}

	// memory_dump();
	memory_check(true);

	exit(0);
}

void indexing(string doc, int index, string url){
	// cout << "\t\tDocument " << index << endl;
	// cout << "URL:\t " << url << endl;
	// cout << "Content: " << doc << endl;
	// cout << "------------------" << endl << endl;	

	//Parse some html code
	// string html = "<html><body>hey, this Is (é) the whole vector<char> v</body></html>";
	htmlcxx::HTML::ParserDom parser;
	tree<htmlcxx::HTML::Node> dom = parser.parseTree(doc);

	//Print whole DOM tree
	// cout << dom << std::endl;

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

	// cout << "Tokenized words" << endl;
	int word_id = 0;
	while (t.size() > 0){
		string token = t.getToken();

		if (token.size()) {

			auto search = vocabulary.find(token);

			// if(search != vocabulary.end()) {
				// Token is in index already

				// Is There an entry for the document?
				// if (inverted_index[token].back().file_index == index){
				// 	inverted_index[token].back().position.push_back(word_id);
				// }
				// else {
				// 	FileList list;
				// 	list.file_index = index;
				// 	list.position.push_back(word_id);

				// 	inverted_index[token].push_back(list);
				// }
			// }
			// else {
				// Token has not been added to the index yet
				// FileList list;
				// list.file_index = index;
				// list.position.push_back(word_id);

				// cout << token << " " << index << " " << word_id << endl;

				// inverted_index.insert(std::make_pair(token,vector<FileList>()));

				// inverted_index[token].push_back(list);

			// }

			// Testinf if token is already in the vocabulary
			if (search == vocabulary.end()){
				vocabulary[token] = word_index;
				word_index++;

				// inverted_index.push_back(vector<FileList>());
			}

			auto search2 = inverted_index.find(token);

			if (search2 == inverted_index.end()){
				inverted_index[token] = vector<FileList>();
			}

			// Testinf if token had already been seen in document index
			if (//inverted_index.size() > 0 &&
				inverted_index[token].size() > 0 &&
				inverted_index[token].back().file_index == index) {
					inverted_index[token].back().position.push_back(word_id);
			}
			else {
				FileList list;
				list.file_index = index;
				list.position.push_back(word_id);

				inverted_index[token].push_back(list);
			}

			// <IDw, IDd, fw, position>
			// <int, int, int, int>
			// 4*4 
			// 36
			memory_usage+=32;

			word_id++;

			memory_check();
		}
	}
}

void memory_check(bool force){

	if (force || memory_usage >= MEMORY_LIMITE) {

		cout << "memory_check " << force << endl;

		int index_size = inverted_index.size();
		fstream f, sorted_f;

		f.open("aux_file", ios::out);
		int count = 0;
		for (auto word : vocabulary){
			for (auto document : inverted_index[word.first]){
				int list_size = document.position.size();
				for (auto pos : document.position){
					// f << "<" << word.second << "," << document.file_index << ","  << list_size << "," << pos << ">" << endl;
					f << word.second << " " << document.file_index << " "  << list_size << " " << pos << endl;
					count++;
				}
			}
		}
		f.close();

		inverted_index.clear();
		memory_usage = 0;

		f.open("aux_file", ios::in);
		sorted_f.open(INDEX_FILE_NAME, ios::out | ios::app);

		string line[4];
		vector<array<string,4>> all_lines;

		for (int i = 0; i < count; i++){
			f >> line[0];
			f >> line[1];
			f >> line[2];
			f >> line[3];

			all_lines.push_back({line[0],line[1],line[2],line[3]});
		}

		sort(begin(all_lines), end(all_lines),
			[](const array<string,4>& A, array<string,4>& B) {
				return (
					(A[0] < B[0]) ||									// Sorting by word id
					((A[0] == B[0]) && (A[1] < B[1])) ||				// Sorting by document id
					((A[0] == B[0]) && (A[1] == B[1]) && (A[3] < B[3]))	// Sorting by position
					);
			});

		for (auto s : all_lines){
			sorted_f << s[0] << " " << s[1] << " " << s[2] << " " << s[3] << endl;
		}

		sorted_f << "_______" << endl;

		f.close();
		sorted_f.close();

		
	}
}