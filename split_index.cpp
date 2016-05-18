#include "func.h"			// Defines are here

#define INDEX_OUT "index/split/sorted"

int main(int argc, const char* argv[]) {  

	fstream in, out;
	int id = 0, index_size = 0, local_size = 0;
	string value;
	// unordered_map<string, string> stopwords;					// <word, id>

	// index.load_index();

	in.open(INDEX_BACKUP_FILE_NAME, ios::in);
	out.open(INDEX_OUT+to_string(id), ios::out);

	if (in.is_open()){
		while (!in.eof()){
			string aux = "";
			for (int i = 0; i < 4; i++){
				in >> value;
				aux+=(value+" ");
			}

			if (aux.back() == ' '){
				aux.pop_back();
			}

			if (!in.eof() && out.is_open()){
				out << aux << "\n";
			}

			local_size++;
			index_size++;

			if (local_size > MEMORY_LIMITE/INDEX_LINE_SIZE){
				out.close();
				id++;
				local_size = 0;
				out.open(INDEX_OUT+to_string(id), ios::out);
			}
		}

		out.close();
	}

	exit(0);

}