#include <iostream>
#include <unordered_map>
#include <cstdlib>
#include <queue>
#include <html/ParserDom.h>
#include "Tokenizer.h"

// #include <boost/algorithm/string.hpp>

#define MEMORY_LIMITE 250000 // (bytes)
// #define MEMORY_LIMITE 160 // (bytes)
// #define MEMORY_LIMITE 8000000000 // (bytes)

/* <IDw, IDd, fw, position>
 * <int, int, int, int>
 * 4*4 
 * 36
*/
#define INDEX_LINE_SIZE 32

#define LOCAL_VOCABULARY_SIZE 30000 //(bytes)


// #define FILENAME "htmls"
#define DIRNAME "/home/pedrinho/Documents/RI/Crawler/70 mins collect/htmls/"
#define STOPWORDS "stopwords/"
#define INDEX_AUX_FILE_NAME "index/aux_index"
#define INDEX_BACKUP_FILE_NAME "index/backup_index"
#define INDEX_SORTED_FILE_NAME "index/sorted_index"
#define VOCABULARY_FILE_NAME "index/vocabulary"

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

int word_index = 0, memory_usage = 0, total_size_index = 0;

string vocabulary_buffer = "";

string parsing(string doc);
void indexing(string doc, int index, string url);
void memory_dump();
void sorted_index();
void vocabulary_dump();

int main(int argc, const char * argv[]) {  

	vector<string> files;
	fstream input;
	string acc, url;
	int pipe_count = 0, state = 0;
	int file_index = 0;
	std::size_t found;

	files = list_dir_files(DIRNAME);

	input.open(INDEX_AUX_FILE_NAME, ios::out);
	input.close();

	input.open(INDEX_BACKUP_FILE_NAME, ios::out);
	input.close();

	input.open(INDEX_SORTED_FILE_NAME, ios::out);
	input.close();

	input.open(VOCABULARY_FILE_NAME, ios::out);
	input.close();

	for (string file : files){
		input.open(file, ios::in);

		if (input.is_open()){
			while (!input.eof()){
				string aux;
				input >> aux;

				switch(state){
					case 0:
						found = aux.find("|||");

						if (found != std::string::npos) {
							state = 1;
						}
						break;
					case 1:
						found = aux.find("|");

						if (found != std::string::npos) {
							state = 2;
						} else {
							url+=aux+" ";
						}
						break;
					case 2:
						found = aux.find("|||");

						if (found != std::string::npos) {
							state = 1;

							indexing(acc, file_index, url);
							file_index++;

							acc = "";
							url = "";

						} else {
							acc+=aux+" ";
						}
						break;
				}
			}
		}

		input.close();
	}

	// for (auto word : vocabulary){
	// 	cout << "Word: \"" << word.first << "\"" << endl;

	// 	for (auto document : inverted_index[word.first]){
	// 		cout << "  Document " << document.file_index << endl;
	// 		cout << "\t";
	// 		for (auto file : document.position){
	// 			cout << file << " ";
	// 		}
	// 		cout << endl;
	// 	}
	// 	cout << endl;
	// }

	// memory_dump();
	sorted_index();
	vocabulary_dump();

	exit(0);
}

//Parse doc's html code
string parsing(string doc){
	string text = "";

	htmlcxx::HTML::ParserDom parser;
	tree<htmlcxx::HTML::Node> dom = parser.parseTree(doc);

	tree<htmlcxx::HTML::Node>::iterator it = dom.begin();

	for (; it != dom.end(); ++it) {
		if(it.node != 0 && dom.parent(it) != NULL){
			string tag_name = dom.parent(it)->tagName();
			transform(tag_name.begin(), tag_name.end(), tag_name.begin(), ::tolower);

			// Skipping code embedded in html
			if ((tag_name == "script") ||
				(tag_name == "noscript")
				){
				it.skip_children();
				continue;
			}
		}

		if ((!it->isTag()) && (!it->isComment())) {
			text.append(it->text()+" ");
			// cout << it->text()+" ";
		}
	}

	// cout << text << endl;

	return text;
}

void indexing(string doc, int index, string url){
	
	// Generating tokens
	Tokenizer t(parsing(doc));

	// Iterating through tokens
	int word_id = 0;
	while (t.size() > 0){
		string token = t.getToken();

		if (token.size()) {

			// Testinf if token is not already in the vocabulary
			auto search = vocabulary.find(token);
			if (search == vocabulary.end()){
				vocabulary[token] = word_index;
				word_index++;

				vocabulary_buffer+=(token+'\n');

				if (vocabulary_buffer.length() >= LOCAL_VOCABULARY_SIZE){
					vocabulary_dump();
				}
			}

			// Testinf if token is not already in the index
			auto search2 = inverted_index.find(token);
			if (search2 == inverted_index.end()){
				inverted_index[token] = vector<FileList>();
			}

			// Testinf if token had already been seen in document index
			if (inverted_index[token].size() > 0 &&
				inverted_index[token].back().file_index == index) {
					inverted_index[token].back().position.push_back(word_id);
			}
			else {
				FileList list;
				list.file_index = index;
				list.position.push_back(word_id);

				inverted_index[token].push_back(list);
			}

			memory_usage+=INDEX_LINE_SIZE;

			word_id++;

			if (memory_usage >= MEMORY_LIMITE){
				memory_dump();
			}
		}
	}
}

void memory_dump(){

	int count = 0, index_size = inverted_index.size();
	fstream f, sorted_f;

	f.open(INDEX_AUX_FILE_NAME, ios::out);
	for (auto word : vocabulary){
		for (auto document : inverted_index[word.first]){
			int list_size = document.position.size();
			for (auto pos : document.position){
				f << word.second << " " << document.file_index << " "  << list_size << " " << pos << endl;
				count++;
			}
		}
	}

	f.close();

	inverted_index.clear();
	memory_usage = 0;

	f.open(INDEX_AUX_FILE_NAME, ios::in);
	sorted_f.open(INDEX_BACKUP_FILE_NAME, ios::out | ios::app);

	string value;
	vector<array<int,4>> all_lines;

	for (int i = 0; i < count; i++){
		array<int,4> aux;

		for (int j = 0; j < 4; j++){
			f >> value;
			aux[j] = stoi(value);
		}

		all_lines.push_back(aux);
	}

	sort(begin(all_lines), end(all_lines),
		[](const array<int,4>& A, array<int,4>& B) {
			return (
				(A[0] < B[0]) ||									// Sorting by word id
				((A[0] == B[0]) && (A[1] < B[1])) ||				// Sorting by document id
				((A[0] == B[0]) && (A[1] == B[1]) && (A[3] < B[3]))	// Sorting by position
				);
		});

	for (auto s : all_lines){
		sorted_f << s[0] << " " << s[1] << " " << s[2] << " " << s[3] << endl;
	}

	f.close();
	sorted_f.close();

	total_size_index += count;

}

void sorted_index(){

	if (memory_usage) {
		memory_dump();
	}

	// Deciding number of splits in backup_index	
	int index_split = ((total_size_index % (MEMORY_LIMITE/INDEX_LINE_SIZE)) ?
						(total_size_index/(MEMORY_LIMITE/INDEX_LINE_SIZE)) + 1 :	// In case number is odd
						(total_size_index/(MEMORY_LIMITE/INDEX_LINE_SIZE)));		// In case number is even

	int read_times[index_split];
	fstream sorted_file, pointers[index_split];
	ifstream is;
	priority_queue<array<int,5>, vector<array<int,5>>, comparator> min_heap;
	bool loop_control = false;
	array<int,5> aux;

	// cout << "Max per bucket: " << MEMORY_LIMITE/INDEX_LINE_SIZE << endl;
	// cout << "Number of buckets: " << index_split << endl;

	sorted_file.open(INDEX_SORTED_FILE_NAME, ios::out);

	for (int i = 0; i < index_split; i++){
		pointers[i].open(INDEX_BACKUP_FILE_NAME, ios::in);
		read_times[i] = 0;
	}

	string value;

	for (int i = 1; i < index_split; i++){
		for(int k = 0; k < (MEMORY_LIMITE/INDEX_LINE_SIZE)*i; k++){
			for (int j = 0; j < 4; j++){
				pointers[i] >> value;
			}
		}
	}


	for (int i = 0; i < index_split; i++){
		aux[4]=i;
		for (int j = 0; j < 4; j++){
			pointers[i] >> value;
			aux[j] = stoi(value);
		}

		if (!pointers[i].eof()){
			min_heap.push(aux);
			read_times[i]++;
		}
	}

	while (min_heap.size()){
		aux = min_heap.top();
		min_heap.pop();

		sorted_file << "<";
		for (int i = 0; i < 3; i++){
			sorted_file << aux[i] << "," ;
		}

		sorted_file << aux[3] << ">" << endl;

		// cout << (MEMORY_LIMITE/INDEX_LINE_SIZE) << endl;

		// if (read_times[aux[4]] < (MEMORY_LIMITE/INDEX_LINE_SIZE) && (read_times[aux[4]] < total_size_index)){
		if (read_times[aux[4]] <= total_size_index/index_split){

			// cout << read_times[aux[4]] << endl;

			for (int j = 0; j < 4; j++){
				pointers[aux[4]] >> value;
				aux[j] = stoi(value);
			}

			if (!pointers[aux[4]].eof()){
				min_heap.push(aux);
				read_times[aux[4]]++;
			}
		}

	}

	for (int i = 0; i < index_split; i++){
		pointers[i].close();
	}

	sorted_file.close();
}

void vocabulary_dump(){

	fstream f;

	f.open(VOCABULARY_FILE_NAME, ios::out | ios::app);

	f << vocabulary_buffer;
	
	vocabulary_buffer = "";

	f.close();

}
