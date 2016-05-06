#ifndef FUNC_H
#define FUNC_H

#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <fstream>
#include <algorithm>

using namespace std;

void cleaningWord(string& str);
int partition(vector<array<string,4>>& arr, const int left, const int right);
void quickSort(vector<array<string,4>>& arr, const int left, const int right, const int sz);
bool compareData(const array<string,4>& A, array<string,4>& B);

#endif