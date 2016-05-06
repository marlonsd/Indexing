#include "func.h"

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

// void quickSort(vector<array<string,4>>& arr, int left, int right) {
// 	int i = left, j = right;
// 	array<string,4> tmp, pivot = arr[left + (right - left) / 2];
 
// 	/* partition */
// 	while (i <= j) {
// 		while (arr[i][0] < pivot[0]){
// 			i++;
// 		}

// 		while (arr[j][0] > pivot[0]){
// 			j--;
// 		}

// 		if (i <= j) {
// 			tmp = arr[i];
// 			arr[i] = arr[j];
// 			arr[j] = tmp;
// 			i++;
// 			j--;
// 		}
// 	};
 
// 	/* recursion */
// 	if (left < j) {
// 		quickSort(arr, left, j);
// 	}

// 	if (i < right) {
// 		quickSort(arr, i, right);
// 	}
// }
 
int partition(vector<array<string,4>>& arr, const int left, const int right) {
	const int mid = left + (right - left) / 2;
	const array<string,4> pivot = arr[mid];

	// move the mid point value to the front.
	std::swap(arr[mid],arr[left]);
	int i = left + 1;
	int j = right;

	while (i <= j) {
		while(i <= j && arr[i][0] <= pivot[0]) {
			i++;
		}
 
		while(i <= j && arr[j][0] > pivot[0]) {
			j--;
		}
 
		if (i < j) {
			std::swap(arr[i], arr[j]);
		}
	}
	std::swap(arr[i - 1],arr[left]);
	return i - 1;
}
 
void quickSort(vector<array<string,4>>& arr, const int left, const int right, const int sz){
 
	if (left >= right) {
		return;
	}
 
	int part = partition(arr, left, right);
 
	quickSort(arr, left, part - 1, sz);
	quickSort(arr, part + 1, right, sz);
}

bool compareData(const array<string,4>& A, array<string,4>& B) {
	return ((A[0] < B[0]) || ((A[0] == B[0]) && (A[1] < B[1])));
		   // ((Records[A].Age == Records[B].Age) && (Records[A].Gender == Records[B].Gender) &&
			  // (strcmp(Records[A].Surname, Records[B].Surname) < 0));
}