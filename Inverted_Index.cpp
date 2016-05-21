#include "Inverted_Index.h"

InvertedIndex::InvertedIndex(){
	this->vocabulary_buffer = "";
	this->memory_usage = 0;
	this->word_index = 0;
	this->total_token = 0;
	this->total_size_index = 0;
	this->n_dumps = 0;

	this->vocabulary = {};
	this->inverted_index = {};
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
	sorted_f.open(INDEX_BACKUP_FILE_NAME+to_string(this->n_dumps), ios::out);
	// sorted_f.open(INDEX_BACKUP_FILE_NAME, ios::out | ios::app);

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
	this->n_dumps++;
}

// void InvertedIndex::sorted_index(){

// 	if (this->memory_usage) {
// 		memory_dump();
// 	}
// 	// this->total_size_index = 91347974;
// 	// Total Number of buckets
// 	// Deciding number of splits in backup_index	
// 	int index_split = ((this->total_size_index % (MEMORY_LIMITE/INDEX_LINE_SIZE)) ?
// 						(this->total_size_index/(MEMORY_LIMITE/INDEX_LINE_SIZE)) + 1 :	// In case number is odd
// 						(this->total_size_index/(MEMORY_LIMITE/INDEX_LINE_SIZE)));		// In case number is even
// 	cout << "# Buckets " << index_split << endl;
// 	if (index_split >= MEMORY_LIMITE/INDEX_LINE_SIZE){
// 		cout << "Too many things" << endl;
// 	}

// 	int read_times[index_split];
// 	cout << "Inst ok" << endl;	
// 	fstream sorted_file;
// 	cout << "Inst ok" << endl;	
// 	ifstream pointers[index_split];
// 	cout << "Inst ok" << endl;	
// 	priority_queue<array<int,5>, vector<array<int,5>>, comparator> min_heap;
// 	cout << "Inst ok" << endl;	
// 	bool loop_control = false;
// 	cout << "Inst ok" << endl;	
// 	array<int,5> aux;

// 	// cout << "Max per bucket: " << MEMORY_LIMITE/INDEX_LINE_SIZE << endl;
// 	// cout << "Number of buckets: " << index_split << endl;

// 	string value;

// 	cout << "Inst ok" << endl;	

// 	pointers[0].open(INDEX_BACKUP_FILE_NAME);
// 	read_times[0] = 0;

// 	cout << "First ok" << endl;

// 	for (int i = 1; i < index_split; i++){
// 		cout << i << endl;
// 		pointers[i].open(INDEX_BACKUP_FILE_NAME);
// 		read_times[i] = 0;

// 		pointers[i].seekg(pointers[i-1].tellg());

// 		for(int k = 0; k < (MEMORY_LIMITE/INDEX_LINE_SIZE); k++){
// 			for (int j = 0; j < 4; j++){
// 				if (!pointers[i].eof()){
// 					pointers[i] >> value;
// 					cout << value << " ";
// 				}
// 			}
// 		}
// 	}

// 	cout << "Correctly positioned" << endl;
// 	cout << endl;

// 	for (int i = 0; i < index_split; i++){
// 		aux[4]=i;
// 		for (int j = 0; j < 4; j++){
// 			pointers[i] >> value;
// 			aux[j] = stoi(value);
// 		}

// 		if (!pointers[i].eof()){
// 			min_heap.push(aux);
// 			read_times[i]++;
// 		}
// 	}

// 	sorted_file.open(INDEX_SORTED_FILE_NAME, ios::out);

// 	while (min_heap.size()){
// 		aux = min_heap.top();
// 		min_heap.pop();

// 		sorted_file << "<";
// 		for (int i = 0; i < 3; i++){
// 			sorted_file << aux[i] << "," ;
// 		}

// 		sorted_file << aux[3] << ">" << '\n';

// 		// cout << (MEMORY_LIMITE/INDEX_LINE_SIZE) << endl;

// 		// if (read_times[aux[4]] < (MEMORY_LIMITE/INDEX_LINE_SIZE) && (read_times[aux[4]] < total_size_index)){
// 		if (read_times[aux[4]] <= this->total_size_index/index_split){

// 			// cout << read_times[aux[4]] << endl;

// 			for (int j = 0; j < 4; j++){
// 				pointers[aux[4]] >> value;
// 				aux[j] = stoi(value);
// 			}

// 			if (!pointers[aux[4]].eof()){
// 				min_heap.push(aux);
// 				read_times[aux[4]]++;
// 			}
// 		}

// 	}

// 	for (int i = 0; i < index_split; i++){
// 		pointers[i].close();
// 	}

// 	sorted_file.close();
// }

void InvertedIndex::sorted_index(){
	int i = 0;
	array<int,5> aux;
	string value;
	priority_queue<array<int,5>, vector<array<int,5>>, comparator> min_heap;

	if (this->memory_usage){
		memory_dump();
	}


	//this->total_size_index = 114543866;
	//this->total_token = 114543866;
	//this->n_dumps = 14661;

	cout << "Total tokens: " << this->total_size_index << " " << this->total_token << endl;
	cout << "Memory Limit: " << (MEMORY_LIMITE/INDEX_LINE_SIZE) << endl;
	cout << "Total of files: " << this->n_dumps << endl;


	while(i < this->n_dumps){
		int n_files; // = (((this->n_dumps - i) < (MEMORY_LIMITE/INDEX_LINE_SIZE)) ?
						// this->n_dumps - i : 
						// (MEMORY_LIMITE/INDEX_LINE_SIZE));
		ofstream out;

		if ((this->n_dumps - i) <= (MEMORY_LIMITE/INDEX_LINE_SIZE)){
			n_files = this->n_dumps - i;
			out.open(INDEX_SORTED_FILE_NAME, ofstream::out);
			cout << INDEX_SORTED_FILE_NAME << endl;
		} else {
			n_files = (MEMORY_LIMITE/INDEX_LINE_SIZE);
			out.open(INDEX_BACKUP_FILE_NAME+to_string(this->n_dumps), ofstream::out);
			cout << INDEX_BACKUP_FILE_NAME+to_string(this->n_dumps) << endl;
			this->n_dumps++;
		}

		if (n_files > (MAX_OS_OPEN_FILE - 1)){
			n_files = MAX_OS_OPEN_FILE-1;
		}

		int count[n_files];

		cout << n_files << endl;
		cout << "Evaliating from " << i << " to " << i+n_files << endl << endl; 

		ifstream p[n_files];
		
		for (int j = 0; j < n_files; j++){
			p[j].open(INDEX_BACKUP_FILE_NAME+to_string(i));
			i++;

			// Reading line
			for (int k = 0; k < 4; k++){
				p[j] >> value;
				aux[k] = stoi(value);
			}

			aux[4] = j; // Locating read file
			count[j] = 0;

			// File hasn't ended yet
			if (!p[j].eof()){
				min_heap.push(aux);
			}
		}

		cout << min_heap.size() << endl;
		int heap_size = 1;
		while (min_heap.size()){

			// Getting smallest tuple
			aux = min_heap.top();
			min_heap.pop();

			// Saving smallest tuple
			for (int j = 0; j < 3; j++){
				out << aux[j] << " " ;
			}
			out << aux[3] << '\n';
			//cout << heap_size << endl;
			//heap_size++;
			//count[aux[4]]++;
			//cout << aux[4] << " " << count[aux[4]] << " [ "<< aux[0] << " " << aux[1] << " " << aux[2] << " " << aux[3] << " ]" << endl;
			// Testing if file hasn't ended yet
			if (!p[aux[4]].eof()){

				for (int j = 0; j < 4; j++){
					p[aux[4]] >> value;
					aux[j] = stoi(value);
				}

				if (!p[aux[4]].eof()){
					min_heap.push(aux);
				}
			} else {
				cout << "File " << aux[4] << " done." << endl;
			}

		}


		for (int j = 0; j < n_files; j++){
			p[j].close();
			// TODO: DELETE FILE
			//string filename = INDEX_BACKUP_FILE_NAME+to_string(i-n_files+j);
			//remove(filename.c_str());
				

			// cout << "Removing file " << INDEX_BACKUP_FILE_NAME+to_string(i-n_files+j) << endl;
		}

		out.close();
	}
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

	// cout << word << endl;

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
			string aux = "";
			entry = "";

			for (int i = 0; i < 3; i++){
				f >> aux;
				entry += aux + ",";
			}

			f >> aux;
			entry += aux;

			// f >> split_entry[0] >> split_entry[1] >> split_entry[2] >> split_entry[3];

			cout << entry << endl;

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

				split_entry.clear();
			}
		}
	}
}

void InvertedIndex::load_index(){
	// cout << "Loading vocabulary" << endl;
	this->load_vocabulary();
	// cout << "Vocabulary size = " << this->vocabulary.size() << endl;
	// this->load_full_index();
}

vector<FileList> InvertedIndex::get_list(string& token){
	vector<FileList> list = {};
	int line[4] = {-1,-1,-1,-1};
	ifstream f;
	string s = "";

	auto search = this->vocabulary.find(token);
	if (search != this->vocabulary.end()){
		f.open(INDEX_SORTED_FILE_NAME);

		while(line[0] <= vocabulary[token] && !f.eof()){

			for (int i = 0; i < 4; i++){
				f >> s;
				line[i] = stoi(s);
			}

			if (line[0] == vocabulary[token]){
				FileList temp;
				int rep = line[2];
				temp.file_index = line[1];
				temp.position.push_back(line[3]);

				if (!f.eof()){
					for (int r = 0; r < rep - 1; r++ ){				
						for (int i = 0; i < 4; i++){
							f >> s;
						}

						temp.position.push_back(stoi(s));						
					}
				}

				list.push_back(temp);
			} else {
				for (int r = 0; r < line[2]-1; r++){
					for (int i = 0; i < 4; i++){
						f >> s;
					}
				}
			}
		}

	}

	return list;
}
