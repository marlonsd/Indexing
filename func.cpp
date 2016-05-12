#include "func.h"

//Parse doc's html code
string parsing(string doc){
	string text = "";

	htmlcxx::HTML::ParserDom parser;
	tree<htmlcxx::HTML::Node> dom = parser.parseTree(doc);

	tree<htmlcxx::HTML::Node>::iterator it = dom.begin();

	for (; it != dom.end(); ++it) {
		if(it.node != 0 && dom.parent(it) != NULL){
			string tag_name = dom.parent(it)->tagName();
			transform(tag_name.begin(), tag_name.end(), tag_name.begin(), ::tolower);

			// Skipping code embedded in html
			if ((tag_name == "script") ||
				(tag_name == "noscript")
				){
				it.skip_children();
				continue;
			}
		}

		if ((!it->isTag()) && (!it->isComment())) {
			text.append(it->text()+" ");
		}
	}

	return text;
}

void cleaningWord(string& str) {
	for(unsigned int i=0;i<str.length();i++) {
	  str.at(i) = tolower(str.at(i));
	  unsigned char c = str.at(i);
	  if ((c == 0xc3) && ((i+1)<str.length())) {
		str.erase (i,1);
		c = str.at(i);
		c = tolower(c);
	  }
	  if ((c >= 0x30 && c <= 0x39) || (str.at(i) >= 0x61 && str.at(i) <= 0x7a)) {
		//à, á, â, ã, ä
	  }else if (( c >= 0xa0 && c <= 0xa4)){
		str[i]='a';
		//ç
	  }else if (c == 0xa7) {
		str[i]='c';
		//è, é, ê , ë
	  } else if ((c >= 0xa8 && c <= 0xab)){
		str[i]='e';
		//ì, í, î, ï
	  } else if ((c >= 0xac && c <= 0xaf)){
		str[i]='i';
		//ñ
	  } else if (c == 0xb1) {
		str[i]='n';
		//ò, ó, ô, õ, ö
	  } else if ((c >= 0xb2 && c <= 0xb6)){
		str[i]='o';
		//ù, ú, û, ü
	  } else if ((c >= 0xb9 && c <= 0xbc)){
		str[i]='u';
		//Se nao for espaco
	  }
	  else if(c!=0x20){
		unsigned int x;
		x=str[i];
		if((x>=4294967265)&&(x<=4294967270)){ str[i]='a';}else
			if((x>=4294967272)&&(x<=4294967275)){ str[i]='e';}else
			  if((x>=4294967276)&&(x<=4294967279)) {str[i]='i';}else
				if(((x>=4294967282)&&(x<=4294967287))||(x==4294967280)){ str[i]='o';}else
					if(x==4294967281){ str[i]='n';}else
					  if((x>=4294967289)&&(x<=4294967292)) {str[i]='u';}else
						if(x==4294967271){ str[i]='c';}else{str.replace (i,1," ");}
	  }
	}
}

vector<string> list_dir_files(string path) {

	DIR* dir;
	dirent* pdir;
	vector<std::string> files;

	dir = opendir(path.c_str());

	while (pdir = readdir(dir)) {
		string filename = pdir->d_name;
		if (filename != "." && filename != ".." && filename[0] != '.'){
			files.push_back(filename);
		}
	}

	closedir(dir);

	return files;
}

unordered_set<string> load_stop_words(string path){
	string word;
	fstream input;
	vector<string> files;
	unordered_set<string> stopwords;
	
	files = list_dir_files(path);

	for (string file : files){
		input.open(path+file, ios::in);
		if (input.is_open()){
			while (!input.eof()){
				input >> word;

				if(word.size()){
					stopwords.insert(word);
				}
			}
		}
		input.close();
	}

	return stopwords;
}

