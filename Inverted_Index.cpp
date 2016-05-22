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

	for (int i = 0; i < 4; i++){
		this->previous[i] = 0;
	}
}

InvertedIndex::InvertedIndex(Tokenizer& t, int index){
	InvertedIndex();
	this->indexing(t, index);
}

void InvertedIndex::reset_distance(){
	for (int i = 0; i < 4; i++){
		this->previous[i] = 0;
	}
}

void InvertedIndex::indexing(Tokenizer& t, int index){

	/* A file must be indexed entirely, so it is possible to calculate total number of terms in a same file.
		To do so, it is calculate the necessaary space to store this one file and the actual size of index.
		If it is going to be bigger than memory size, dump index first.
	*/
	if (((t.size()*INDEX_LINE_SIZE)+this->memory_usage) >= MEMORY_LIMITE){
		this->memory_dump();
	}

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

			// Testing if token had already been seen in document index
			if (search2 != this->inverted_index.end() &&					// Testing if token is in index
				this->inverted_index[token].size() > 0 &&
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
				this->memory_dump();
			}
		}
	}
}

void InvertedIndex::memory_dump(){

	int count = 0, index_size = this->inverted_index.size();
	fstream f, sorted_f;

	f.open(INDEX_AUX_FILE_NAME, ios::out);

	// Generating tuples and saving in file
	// < word_id, doc_id, freq, pos >
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

	string value;
	vector<array<int,4>> all_lines;

	// Loading tuples
	for (int i = 0; i < count; i++){
		array<int,4> aux;

		for (int j = 0; j < 4; j++){
			f >> value;
			aux[j] = stoi(value);
		}

		all_lines.push_back(aux);
	}

	// Sorting tuples
	sort(begin(all_lines), end(all_lines),
		[](const array<int,4>& A, array<int,4>& B) {
			return (
				(A[0] < B[0]) ||									// Sorting by word id
				((A[0] == B[0]) && (A[1] < B[1])) ||				// Sorting by document id
				((A[0] == B[0]) && (A[1] == B[1]) && (A[3] < B[3]))	// Sorting by position
				);
		});

	// Saving sorted tuples
	for (auto s : all_lines){
		sorted_f << s[0] << " " << s[1] << " " << s[2] << " " << s[3] << '\n';
	}

	f.close();
	sorted_f.close();

	this->total_size_index += count;
	this->n_dumps++;
}

void InvertedIndex::sorted_index(){
	int i = 0;
	array<int,5> aux;
	string value;
	priority_queue<array<int,5>, vector<array<int,5>>, comparator> min_heap;
	bool final = false;

	if (this->memory_usage){
		memory_dump();
	}

	cout << "Total tokens: " << this->total_size_index << " " << this->total_token << endl;
	cout << "Memory Limit: " << (MEMORY_LIMITE/INDEX_LINE_SIZE) << endl;
	cout << "Total of files: " << this->n_dumps << endl;


	while(i < this->n_dumps){
		int n_files;
		ofstream out;

		// Testing wether is possible to open all files at once
		if ((this->n_dumps - i) <= (MEMORY_LIMITE/INDEX_LINE_SIZE) && ((this->n_dumps - i) < (MAX_OS_OPEN_FILE - 1000))){
			// If true, needs saving final sorted index
			n_files = this->n_dumps - i;
			out.open(INDEX_SORTED_FILE_NAME, ios::out);
			final = true;
		} else {
			// Intercalation
			n_files = ((MAX_OS_OPEN_FILE - 1) < (MEMORY_LIMITE/INDEX_LINE_SIZE) ? 
						MAX_OS_OPEN_FILE - 1 :
						(MEMORY_LIMITE/INDEX_LINE_SIZE));

			out.open(INDEX_BACKUP_FILE_NAME+to_string(this->n_dumps), ios::out);
			this->n_dumps++;
		}

		

		ifstream p[n_files];

		// Evaliating from t to i+n_files
		for (int j = 0; j < n_files; j++){
			p[j].open(INDEX_BACKUP_FILE_NAME+to_string(i));
			i++;

			// Reading line
			for (int k = 0; k < 4; k++){
				p[j] >> value;
				aux[k] = stoi(value);
			}

			aux[4] = j; // Locating read file

			min_heap.push(aux);
		}

		this->reset_distance();

		while (min_heap.size()){

			// Getting smallest tuple
			aux = min_heap.top();
			min_heap.pop();

			// If last file, do compression
			if (final){
				this->distance_diff(aux);
			}
			
			// Saving smallest tuple
			for (int j = 0; j < 3; j++){
				out << aux[j] << " " ;
			}

			out << aux[3] << '\n';

			// Test if file have finished
			if (!p[aux[4]].eof()){

				for (int j = 0; j < 4; j++){
					p[aux[4]] >> value;
					aux[j] = stoi(value);

				}

				// Test if file have finished
				if (!p[aux[4]].eof()){
					min_heap.push(aux);
				}
			}
		}


		for (int j = 0; j < n_files; j++){
			p[j].close();

			// Deleting temp files
			string filename = INDEX_BACKUP_FILE_NAME+to_string(i-n_files+j);
			remove(filename.c_str());				
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

	f.close();
}

void InvertedIndex::load_index(){
	this->load_vocabulary();
}

vector<FileList> InvertedIndex::get_list(string& token){
	vector<FileList> list = {};
	array<int,4> line = {-1,-1,-1,-1};
	ifstream f;
	string s = "";

	this->reset_distance();

	auto search = this->vocabulary.find(token);
	if (search != this->vocabulary.end()){
		f.open(INDEX_SORTED_FILE_NAME);

		while(line[0] <= vocabulary[token] && !f.eof()){

			for (int i = 0; i < 4; i++){
				f >> s;
				line[i] = stoi(s);
			}

			this->distance_rest(line);

			if (line[0] == vocabulary[token]){
				FileList temp;
				int rep = line[2];
				temp.file_index = line[1];
				temp.position.push_back(line[3]);

				if (!f.eof()){
					for (int r = 0; r < rep - 1; r++ ){				
						for (int i = 0; i < 4; i++){
							f >> s;
							line[i] = stoi(s);
						}

						this->distance_rest(line);

						temp.position.push_back(line[3]);						
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

void InvertedIndex::distance_diff(array<int,5>& v){

	// v[0] = Term index
	// v[1] = Doc index
	// v[2] = Term freq at doc 
	// v[3] = Pos at doc

	if (v[1] != this->previous[1]){
		this->previous[2] = 0;
		this->previous[3] = 0;
	}

	if (v[0] != this->previous[0]){
		this->previous[1] = 0;
		this->previous[2] = 0;
		this->previous[3] = 0;
	}

	for (int i = 0; i < 4; i++){
		v[i] = v[i] - this->previous[i];
		this->previous[i] = this->previous[i] + v[i];

	}

}

void InvertedIndex::distance_rest(array<int,4>& v){

	if (v[1] != 0){
		this->previous[2] = 0;
		this->previous[3] = 0;
	}

	if (v[0] != 0){
		this->previous[1] = 0;
		this->previous[2] = 0;
		this->previous[3] = 0;
	}

	for (int i = 0; i < 4; i++){
		v[i] = v[i] + this->previous[i];
		this->previous[i] = v[i];
	}

}

void InvertedIndex::rest(){
	ifstream f;
	array<int,4> aux;
	string s;

	f.open(INDEX_SORTED_FILE_NAME);

	this->reset_distance();

	while(!f.eof()){
		for (int i = 0; i < 4; i++){
			f >> s;
			aux[i] = stoi(s);
		}

		if (!f.eof()){
			distance_rest(aux);

			for (int i = 0; i < 3; i++){
				cout << aux[i] << " ";
			}

			cout << aux[3] << endl;
		}

	}
}