#include <iostream>  
#include <html/ParserDom.h>  
#include "Tokenizer.h"

#define FILENAME "htmls"

void indexing(string doc, int index, string url);

int main(int argc, const char * argv[]) {  

	fstream input;
	string acc, url;
	int pipe_count = 0;
	int file_index = 0;

	input.open(FILENAME, ios::in);

	if (input.is_open()){
		while (!input.eof()){
			string aux;
			input >> aux;

			// cout << aux << endl;

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

	exit(0);
}

void indexing(string doc, int index, string url){
	cout << "\t\tDocument " << index << endl;
	cout << "URL:\t " << url << endl;
	cout << "Content: " << doc << endl;
	cout << "------------------" << endl << endl;	

	//Parse some html code
	std::string html = "<html><body>hey, this Is (é) the whole vector<char> v</body></html>";
	htmlcxx::HTML::ParserDom parser;
	tree<htmlcxx::HTML::Node> dom = parser.parseTree(html);

	//Print whole DOM tree
	std::cout << dom << std::endl;

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

	cout << "Tokenized words" << endl;
	while (t.size() > 0){
		cout << t.getToken() << endl;
	}
}