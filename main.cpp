#include <chrono> // C++11 Time lib
#include "Inverted_Index.h"
#include "func.h"			// Defines are here

using namespace std::chrono;

void resetingOutputFiles();
void parsing(const string& doc, Tokenizer& t, const unordered_set<string>& stopwords);

high_resolution_clock::time_point t0;

int main(int argc, const char* argv[]) {  

	vector<string> files;
	fstream input, doc_id;
	string acc, url, last_read = "";
	int state = 0, file_index = 0;
	size_t found, alt_found;
	unordered_set<string> stopwords = load_stop_words(STOPWORDS_PATH);
	InvertedIndex index;
	double duration;
	Tokenizer t;

	high_resolution_clock::time_point t1, t2;

	resetingOutputFiles();

	files = list_dir_files(DIRNAME);

	doc_id.open(DOC_ID_FILE_NAME, ios::out);

	// Time program started
	t0 = high_resolution_clock::now();

	cout << "reading (ms),tokenizing (ms),indexing (ms),#files,total time (s), sorting, voc dump" << endl;



	for (string file : files){
		input.open(DIRNAME+file, ios::in);

		if (input.is_open()){
			t1 = high_resolution_clock::now();
			while (!input.eof()){
				string aux;
				input >> aux;

				switch(state){
					case 0:
						if (aux == "|||"){
							state = 1;
						}

						break;

					case 1:
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

						// 7 is "</html>" size
						if (found != std::string::npos && (aux.size() - found) == 7){
							state = 4;
						}

						break;

					case 4:

						if (aux == "|||"){

							doc_id << url << endl;

							t2 = high_resolution_clock::now();

							duration = duration_cast<milliseconds>( t2 - t1 ).count();

							cout << duration << ",";

							t1 = high_resolution_clock::now();
							parsing(acc, t, stopwords);
							// Tokenizer t(parsing(acc), stopwords);
							t2 = high_resolution_clock::now();

							duration = duration_cast<milliseconds>( t2 - t1 ).count();

							cout << duration << ",";

							t1 = high_resolution_clock::now();
							index.indexing(t, file_index);
							t2 = high_resolution_clock::now();

							duration = duration_cast<milliseconds>( t2 - t1 ).count();
							cout << duration << ",";
							file_index++;

							duration = duration_cast<seconds>( t2 - t0 ).count();

							cout << file_index << "," << duration << endl;


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

	doc_id.close();

	cout << "Done indexing" << endl;

	t1 = high_resolution_clock::now();
	index.sorted_index();
	t2 = high_resolution_clock::now();

	duration = duration_cast<milliseconds>( t2 - t1 ).count();

	cout << duration << ",";

	t1 = high_resolution_clock::now();
	index.vocabulary_dump();
	t2 = high_resolution_clock::now();

	duration = duration_cast<milliseconds>( t2 - t1 ).count();
	
	cout << duration << ",";

	exit(0);
}

void resetingOutputFiles(){
	fstream output;
	
	output.open(INDEX_AUX_FILE_NAME, ios::out);
	output.close();

	output.open(INDEX_SORTED_FILE_NAME, ios::out);
	output.close();

	output.open(VOCABULARY_FILE_NAME, ios::out);
	output.close();
}

//Parse doc's html code
void parsing(const string& doc, Tokenizer& t, const unordered_set<string>& stopwords){
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
				// (tag_name == "style") ||
				(tag_name == "textarea")
				){
				it.skip_children();
				continue;
			}
		}

		if ((!it->isTag()) && (!it->isComment())) {
			t.addTokens(it->text(), stopwords);
		}
	}
}