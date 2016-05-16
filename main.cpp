#include "Inverted_Index.h"
#include "func.h"			// Defines are here

void resetingOutputFiles();

int main(int argc, const char* argv[]) {  

	vector<string> files;
	fstream input, doc_id;
	string acc, url, last_read = "";
	int state = 0, count = 0;
	int file_index = 0;
	size_t found;
	unordered_set<string> stopwords = load_stop_words(STOPWORDS_PATH);
	InvertedIndex index;

	resetingOutputFiles();

	files = list_dir_files(DIRNAME);

	doc_id.open(DOC_ID_FILE_NAME, ios::out);

	for (string file : files){
		input.open(DIRNAME+file, ios::in);

		if (input.is_open()){
			while (!input.eof()){
				string aux;
				if (!last_read.size()){
					input >> aux;
				} else {
					aux = last_read;
					last_read = "";
				}

				switch(state){
					case 0:
						found = aux.find("|||");

						if (found != std::string::npos) {
							count++;
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
							count++;
							// cout << count << endl;

							if (!input.eof()) {
								input >> last_read;

								// cout << last_read;
								found = last_read.find("http");
								// cout << found;
								if (found != std::string::npos) {

									state = 1;

									// Saving URL
									doc_id << url << endl;

									Tokenizer t(parsing(acc), stopwords);
									index.indexing(t, file_index);
									file_index++;

									acc = "";
									url = "";
								}
							} else {
								state = 1;

								// Saving URL
								doc_id << url << endl;

								Tokenizer t(parsing(acc), stopwords);
								index.indexing(t, file_index);
								file_index++;

								acc = "";
								url = "";
							}

						} else {
							acc+=aux+" ";
						}
						break;
				}
			}
		}

		input.close();
	}

	doc_id.close();

	index.sorted_index();
	index.vocabulary_dump();

	exit(0);
}

void resetingOutputFiles(){
	fstream output;
	
	output.open(INDEX_AUX_FILE_NAME, ios::out);
	output.close();

	output.open(INDEX_BACKUP_FILE_NAME, ios::out);
	output.close();

	output.open(INDEX_SORTED_FILE_NAME, ios::out);
	output.close();

	output.open(VOCABULARY_FILE_NAME, ios::out);
	output.close();
}