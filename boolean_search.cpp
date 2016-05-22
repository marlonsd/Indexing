#include "Inverted_Index.h"
#include "func.h"			// Defines are here

vector<int> instersection(vector<int> &v1, vector<int> &v2);
void load_doc_id(vector<string>& doc_id);

int main(int argc, const char* argv[]) {  

	string query, token;
	std::size_t found;
	InvertedIndex index;
	Tokenizer tokens;
	vector<FileList> list;
	vector<int> all_files;
	bool first = true;
	vector<string> doc_id;

	load_doc_id(doc_id);

	index.load_index();

	while(true) {
		cout << "Query ('q' to exit): ";
		getline(cin, query);

		if (query == "q"){
			cout << endl << "Exiting..." << endl;			
			break;
		}

		if (query.size() <= 1){
			cout << endl << "ERROR: Query has to have more than one caracter." << endl << endl;
		} else {

			cout << endl << endl << "Processing..." << endl;

			tokens.addTokens(query, load_stop_words(STOPWORDS_PATH));

			cout << endl;

			first = true;

			while(tokens.size()){
				token = tokens.getToken();
				cout << "Token '" << token << "' : " << endl;
				list = index.get_list(token);
				if (list.size()){
					vector<int> files;
					for (auto e : list){
						files.push_back(e.file_index);
						cout << "\t File " << e.file_index << endl;
						cout << "\t\tAppears " << e.position.size() << " times"<< endl;
						cout << "\t\tPosition";
						if (e.position.size() > 1){
							cout << "s";
						}
						cout << " [";
						for (auto p : e.position){
							cout << " " << p;
						}
						cout << " ]" <<endl;
					}

					if (first){
						all_files = files;
						first = !first;
					} else {
						vector<int> new_files = instersection(all_files, files);
						all_files = new_files;
					}
				} else {
					cout << "\t Token was not found." << endl;
				}
			}

			cout << endl << "Documents containing all tokens: " << endl;
			for (int i : all_files){
				cout << i << ". " << doc_id[i] << endl;
			}
			cout << endl;

			all_files.clear();

			query = "";

		}
	}

	exit(0);
}


vector<int> instersection(vector<int> &v1, vector<int> &v2){

    vector<int> v3;

    set_intersection(v1.begin(),v1.end(),v2.begin(),v2.end(),back_inserter(v3));

    return v3;
}

void load_doc_id(vector<string>& doc_id){
	ifstream file;

	file.open(DOC_ID_FILE_NAME, ios::in);

	while(!file.eof()){
		string s;

		file >> s;

		if (s.size()){
			doc_id.push_back(s);
		}
	}
}