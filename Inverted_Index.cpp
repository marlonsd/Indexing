#include "Inverted_Index.h"

InvertedIndex::InvertedIndex(){
	this->vocabulary = {};
	this->inverted_index = {};
	this->vocabulary_buffer = "";
	this->memory_usage = 0;
	this->word_index = 0;
	this->total_token = 0;
	this->total_size_index = 0;
}

InvertedIndex::InvertedIndex(Tokenizer& t, int index){
	InvertedIndex();
	this->indexing(t, index);
}

void InvertedIndex::indexing(Tokenizer& t, int index){
	
	// Iterating through tokens
	int word_id = 0;
	while (t.size() > 0){
		string token = t.getToken();

		if (token.size()) {
			this->total_token++;

			// Testing if token is not already in the vocabulary
			auto search = this->vocabulary.find(token);
			if (search == this->vocabulary.end()){
				this->vocabulary[token] = this->word_index;
				this->word_index++;

				this->vocabulary_buffer+=(token+'\n');

				if (this->vocabulary_buffer.length() >= LOCAL_VOCABULARY_SIZE){
					this->vocabulary_dump();
				}
			}

			// Testing if token is not already in the index
			auto search2 = this->inverted_index.find(token);
			if (search2 == this->inverted_index.end()){
				this->inverted_index[token] = vector<FileList>();
			}

			// Testing if token had already been seen in document index
			if (this->inverted_index[token].size() > 0 &&
				this->inverted_index[token].back().file_index == index) {
					this->inverted_index[token].back().position.push_back(word_id);
			}
			else {
				FileList list;
				list.file_index = index;
				list.position.push_back(word_id);

				this->inverted_index[token].push_back(list);
			}

			this->memory_usage+=INDEX_LINE_SIZE;

			word_id++;

			if (this->memory_usage >= MEMORY_LIMITE){
				memory_dump();
			}
		}
	}
}

void InvertedIndex::memory_dump(){

	int count = 0, index_size = this->inverted_index.size();
	fstream f, sorted_f;

	f.open(INDEX_AUX_FILE_NAME, ios::out);

	for (auto word : this->vocabulary){
		for (auto document : this->inverted_index[word.first]){
			int list_size = document.position.size();
			for (auto pos : document.position){
				f << word.second << " " << document.file_index << " "  << list_size << " " << pos << '\n';
				count++;
			}
		}
	}

	f.close();

	this->inverted_index.clear();
	this->memory_usage = 0;

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
		sorted_f << s[0] << " " << s[1] << " " << s[2] << " " << s[3] << '\n';
	}

	f.close();
	sorted_f.close();

	this->total_size_index += count;

}

void InvertedIndex::sorted_index(){

	if (this->memory_usage) {
		memory_dump();
	}
	// this->total_size_index = 91347974;
	// Total Number of buckets
	// Deciding number of splits in backup_index	
	int index_split = ((this->total_size_index % (MEMORY_LIMITE/INDEX_LINE_SIZE)) ?
						(this->total_size_index/(MEMORY_LIMITE/INDEX_LINE_SIZE)) + 1 :	// In case number is odd
						(this->total_size_index/(MEMORY_LIMITE/INDEX_LINE_SIZE)));		// In case number is even
	cout << "# Buckets " << index_split << endl;
	if (index_split >= MEMORY_LIMITE/INDEX_LINE_SIZE){
		cout << "Too many things" << endl;
	}

	int read_times[index_split];
	cout << "Inst ok" << endl;	
	fstream sorted_file;
	cout << "Inst ok" << endl;	
	ifstream pointers[index_split];
	cout << "Inst ok" << endl;	
	priority_queue<array<int,5>, vector<array<int,5>>, comparator> min_heap;
	cout << "Inst ok" << endl;	
	bool loop_control = false;
	cout << "Inst ok" << endl;	
	array<int,5> aux;

	// cout << "Max per bucket: " << MEMORY_LIMITE/INDEX_LINE_SIZE << endl;
	// cout << "Number of buckets: " << index_split << endl;

	string value;

	cout << "Inst ok" << endl;	

	pointers[0].open(INDEX_BACKUP_FILE_NAME);
	read_times[0] = 0;

	cout << "First ok" << endl;

	for (int i = 1; i < index_split; i++){
		cout << i << endl;
		pointers[i].open(INDEX_BACKUP_FILE_NAME);
		read_times[i] = 0;

		pointers[i].seekg(pointers[i-1].tellg());

		for(int k = 0; k < (MEMORY_LIMITE/INDEX_LINE_SIZE); k++){
			for (int j = 0; j < 4; j++){
				if (!pointers[i].eof()){
					pointers[i] >> value;
					cout << value << " ";
				}
			}
		}
	}

	cout << "Correctly positioned" << endl;
	cout << endl;

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

	sorted_file.open(INDEX_SORTED_FILE_NAME, ios::out);

	while (min_heap.size()){
		aux = min_heap.top();
		min_heap.pop();

		sorted_file << "<";
		for (int i = 0; i < 3; i++){
			sorted_file << aux[i] << "," ;
		}

		sorted_file << aux[3] << ">" << '\n';

		// cout << (MEMORY_LIMITE/INDEX_LINE_SIZE) << endl;

		// if (read_times[aux[4]] < (MEMORY_LIMITE/INDEX_LINE_SIZE) && (read_times[aux[4]] < total_size_index)){
		if (read_times[aux[4]] <= this->total_size_index/index_split){

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

void InvertedIndex::vocabulary_dump(){

	fstream f;

	f.open(VOCABULARY_FILE_NAME, ios::out | ios::app);

	f << this->vocabulary_buffer;
	
	this->vocabulary_buffer = "";

	f.close();

}

vector<string> InvertedIndex::get_vocabulary(){
	string word;
	vector<string> vocabulary;
	fstream f;

	f.open(VOCABULARY_FILE_NAME, ios::in);

	if (f.is_open()){
		while (!f.eof()){
			f >> word;

			vocabulary.push_back(word);
		}
	}

	f.close();

	return vocabulary;
}

void InvertedIndex::load_vocabulary(){
	int word_index = 0;
	string word;
	fstream f;

	f.open(VOCABULARY_FILE_NAME, ios::in);

	if (f.is_open()){
		while (!f.eof()){
			f >> word;

			this->vocabulary[word] = word_index;
			word_index++;
		}
	}

	cout << word << endl;

	f.close();
}

void InvertedIndex::load_full_index(){
	string entry;
	vector<string> vocabulary, split_entry;
	fstream f;

	f.open(INDEX_SORTED_FILE_NAME, ios::in);

	if (f.is_open()){
		vocabulary = this->get_vocabulary();

		while(!f.eof()){
			f >> entry;

			if (entry.size() >= 9){
				// Removing "<" and ">"
				if (entry.back() == '>'){
					entry.pop_back();
				}

				if (entry.front() == '<'){
					entry.erase(entry.begin());
				}

				// <word id, doc id, frequency of word, position>
				split(entry, ',' , split_entry);

				if (stoi(split_entry[0]) < vocabulary.size()){
					string token = vocabulary[stoi(split_entry[0])];
					int index = stoi(split_entry[1]);
					int word_id = stoi(split_entry[3]);

					// Testinf if token is not already in the index
					auto search = this->inverted_index.find(token);
					if (search == this->inverted_index.end()){
						this->inverted_index[token] = vector<FileList>();
					}

					// Testinf if token had already been seen in document index
					if (this->inverted_index[token].size() > 0 &&
						this->inverted_index[token].back().file_index == index) {
							this->inverted_index[token].back().position.push_back(word_id);
					}
					else {
						FileList list;
						list.file_index = index;
						list.position.push_back(word_id);

						this->inverted_index[token].push_back(list);
					}
				}

				split_entry.clear();
			}
		}
	}
}

void InvertedIndex::load_index(){
	cout << "Loading vocabulary" << endl;
	this->load_vocabulary();
	cout << "Vocabulary size = " << this->vocabulary.size() << endl;
	this->load_full_index();
}

vector<FileList> InvertedIndex::get_list(string& token){
	vector<FileList> list;
	auto search = this->inverted_index.find(token);

	if (search != this->inverted_index.end()){
		list = this->inverted_index[token];
	}

	return list;
}
