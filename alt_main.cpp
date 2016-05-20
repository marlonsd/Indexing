#include <chrono> // C++11 Time lib
#include "Inverted_Index.h"
#include "func.h"			// Defines are here

using namespace std::chrono;

void parsing(const string& doc, Tokenizer& t, const unordered_set<string>& stopwords);

high_resolution_clock::time_point t0;

int main(int argc, const char* argv[]) {  

	vector<string> files;
	fstream input, doc_id;
	string acc = "", url = "", last_read = "";
	char previous_cond = 0;
	int state = 0, file_index = 0;
	size_t found, alt_found;
	unordered_set<string> stopwords = load_stop_words(STOPWORDS_PATH);
	InvertedIndex index;
	double duration;
	Tokenizer t;

	high_resolution_clock::time_point t1, t2;

	files = list_dir_files(DIRNAME);

	doc_id.open(DOC_ID_FILE_NAME, ios::out);

	// Time program started
	t0 = high_resolution_clock::now();

	// cout << "reading (ms),tokenizing (ms),indexing (ms),#files,total time (s), sorting, voc dump" << endl;

	sort(files.begin(), files.end(),
		[](const string& A, string& B) {
			vector<string> aux1, aux2;
			split(A,'-', aux1);
			split(B,'-', aux2);
			return (stoi(aux1[1]) < stoi(aux2[1]));
		});

	for (string file : files){
		input.open(DIRNAME+file, ios::in);

		cout << "File:" << file << endl;

		if (input.is_open()){
			t1 = high_resolution_clock::now();
			while (!input.eof()){
				string aux;
				input >> aux;

				// cout << aux << endl;

				switch(state){
					case 0:
						// found = aux.find("|||");

						// if (found != std::string::npos){
						if (aux == "|||"){
							state = 1;
						}

						break;

					case 1:
						// found = aux.find("|");

						if (aux == "|"){
							state = 2;
						} else {
							url+=aux+" ";
						}

						break;

					case 2:
						transform(aux.begin(), aux.end(), aux.begin(), ::tolower);

						found = aux.find("<html");
						alt_found = aux.find("<!doctype");

						if ((found != std::string::npos && found == 0) ||
							(alt_found != std::string::npos && alt_found == 0)){
							acc = aux+" ";
							state = 3;
						}

						if (aux == "|||"){
							state = 1;
							url = "";
						}

						break;

					case 3:

						found = aux.find("</html>");

						acc+=aux+" ";

						// 7 is "</html> size"
						if (found != std::string::npos && (aux.size() - found) == 7){
							state = 4;
						}

						break;

					case 4:

						if (aux == "|||"){

							// DO INDEXING

							cout << "FIM DO HTML DE " << url << endl;

							state = 1;
							acc = "";
							url = "";
						}

						break;
				}
			}
		}

		input.close();
	}

	exit(0);
}

//Parse doc's html code
void parsing(const string& doc, Tokenizer& t, const unordered_set<string>& stopwords){
	// string text = "";

	htmlcxx::HTML::ParserDom parser;
	tree<htmlcxx::HTML::Node> dom = parser.parseTree(doc);

	tree<htmlcxx::HTML::Node>::iterator it = dom.begin();

	for (; it != dom.end(); ++it) {
		if(it.node != 0 && dom.parent(it) != NULL){
			string tag_name = dom.parent(it)->tagName();
			transform(tag_name.begin(), tag_name.end(), tag_name.begin(), ::tolower);
			// boost::algorithm::to_lower(tag_name);

			// Skipping code embedded in html
			if ((tag_name == "script") ||
				(tag_name == "noscript") ||
				(tag_name == "style")
				){
				it.skip_children();
				continue;
			}
		}

		if ((!it->isTag()) && (!it->isComment())) {
			// text.append(it->text()+" ");
			t.addTokens(it->text(), stopwords);
		}
	}

	// return text;
}