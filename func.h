#ifndef FUNC_H
#define FUNC_H

#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <cstdlib>
#include <queue>
#include <dirent.h>
#include <html/ParserDom.h>

// #define MEMORY_LIMITE 250000 // (bytes)
#define MEMORY_LIMITE 160 // (bytes)

/* <word id, doc id, frequency of word, position>
 * <int, int, int, int>
 * 4*4 
 * 36
*/
#define INDEX_LINE_SIZE 32

#define LOCAL_VOCABULARY_SIZE 30000 //(bytes)

// #define DIRNAME "../coleta_marlon/"
#define DIRNAME "htmls/"
#define STOPWORDS_PATH "stopwords/"
#define INDEX_AUX_FILE_NAME "index/aux_index"
#define INDEX_BACKUP_FILE_NAME "index/backup_index"
#define INDEX_SORTED_FILE_NAME "index/sorted_index"
#define VOCABULARY_FILE_NAME "index/vocabulary"
#define DOC_ID_FILE_NAME "index/doc_id"


using namespace std;

struct comparator {
	bool operator()(array<int,5> A, array<int,5> B) {
		return (
				(A[0] > B[0]) ||									// Sorting by word id
				((A[0] == B[0]) && (A[1] > B[1])) ||				// Sorting by document id
				((A[0] == B[0]) && (A[1] == B[1]) && (A[3] > B[3]))	// Sorting by position
				);
	}
};

struct FileList {
	int file_index;
	vector<int> position;

	bool operator==(const FileList &other) const {
		return (file_index == other.file_index
				&& position == other.position);
	}
};

void split(const string& s, char c, vector<string>& v);
string parsing(string doc);
void cleaningWord(string& str);
vector<string> list_dir_files(string path);
unordered_set<string> load_stop_words(string path);

#endif