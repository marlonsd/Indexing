#ifndef FUNC_H
#define FUNC_H

#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <fstream>
#include <algorithm>
#include <dirent.h>

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

void cleaningWord(string& str);
vector<string> list_dir_files(string path);
unordered_set<string> load_stop_words(string path);

#endif