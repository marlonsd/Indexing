FLAGS += -funsigned-char -I /usr/local/include/htmlcxx -L/usr/local/lib -lhtmlcxx

all: indexing bsearch

indexing: main.o tokenizer.o inverted_index.o func.o
	g++ -std=c++11 func.o Inverted_Index.o Tokenizer.o main.o $(FLAGS) -o indexing

main.o: main.cpp Tokenizer.h func.h Inverted_Index.h
	g++ -std=c++11 $(FLAGS) -c main.cpp

inverted_index.o: Inverted_Index.cpp Inverted_Index.h Tokenizer.h func.h
	g++ -std=c++11 $(FLAGS) -c Inverted_Index.cpp

tokenizer.o: Tokenizer.cpp Tokenizer.h func.h
	g++ -std=c++11 $(FLAGS) -c Tokenizer.cpp

func.o: func.cpp func.h Tokenizer.h
	g++ -std=c++11 $(FLAGS) -c func.cpp		

clean:
	rm *.o indexing search split

bsearch: bsearch.o tokenizer.o inverted_index.o func.o
	g++ -std=c++11 func.o Inverted_Index.o Tokenizer.o boolean_search.o $(FLAGS) -o search

bsearch.o: boolean_search.cpp Tokenizer.h func.h Inverted_Index.h
	g++ -std=c++11 $(FLAGS) -c boolean_search.cpp

split: split.o tokenizer.o inverted_index.o func.o
	g++ -std=c++11 func.o Inverted_Index.o Tokenizer.o split_index.o $(FLAGS) -o split

split.o: split_index.cpp Tokenizer.h func.h Inverted_Index.h
	g++ -std=c++11 $(FLAGS) -c split_index.cpp

dir:
	mkdir htmls index index/split stopwords

clear: html_sanity.o tokenizer.o inverted_index.o func.o
	g++ -std=c++11 func.o Inverted_Index.o Tokenizer.o clean_html.o $(FLAGS) -o sanity

html_sanity.o: clean_html.cpp Tokenizer.h func.h Inverted_Index.h
	g++ -std=c++11 $(FLAGS) -c clean_html.cpp

alt: alt.o tokenizer.o inverted_index.o func.o
	g++ -std=c++11 func.o Inverted_Index.o Tokenizer.o alt_main.o $(FLAGS) -o alt

alt.o: alt_main.cpp Tokenizer.h func.h Inverted_Index.h
	g++ -std=c++11 $(FLAGS) -c alt_main.cpp

# Run in ubuntu: export LD_LIBRARY_PATH="/usr/local/lib"
#ulimit -n MAX OPEN FILE

# ./indexing_full > full_index.out && ./indexing > short_index.out

