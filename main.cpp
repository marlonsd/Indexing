#include <iostream>  
#include <html/ParserDom.h>  
#include "Tokenizer.h"

int main(int argc, const char * argv[]) {  

	//Parse some html code
	std::string html = "<html><body>hey, this the whole vector<char> v</body></html>";
	htmlcxx::HTML::ParserDom parser;
	tree<htmlcxx::HTML::Node> dom = parser.parseTree(html);

	//Print whole DOM tree
	std::cout << dom << std::endl;

	//Dump all links in the tree
	tree<htmlcxx::HTML::Node>::iterator it = dom.begin();
	tree<htmlcxx::HTML::Node>::iterator end = dom.end(); 

	for (; it != end; ++it) {
		cout << it->tagName() << endl;
		if (it->tagName() == "A") {
			it->parseAttributes();
			std::cout << it->attributes("href");
		}
	}

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