UNAME_S := $(shell uname -s)
TOP := $(shell pwd)
# ifeq ($(UNAME_S),Linux)
# 	FLAGS += -I /usr/local/include/htmlcxx -L/usr/local/lib -lhtmlcxx
# endif
# ifeq ($(UNAME_S),Darwin)
# 	FLAGS += -I /usr/local/include/htmlcxx -L/usr/local/lib -lhtmlcxx
# endif

FLAGS += -I /usr/local/include/htmlcxx -L/usr/local/lib -lhtmlcxx

indexing: main.o tokenizer.o func.o
	g++ -std=c++11 func.o Tokenizer.o main.o $(FLAGS) -o indexing

main.o: main.cpp Tokenizer.h func.h
	g++ -std=c++11 $(FLAGS) -c main.cpp

tokenizer.o: Tokenizer.cpp Tokenizer.h func.h
	g++ -std=c++11 -c Tokenizer.cpp

func.o: func.cpp func.h
	g++ -std=c++11 -c func.cpp		

clean:
	rm *.o indexing

# dir:
# 	mkdir htmls logs backup cache

# Run in ubuntu: export LD_LIBRARY_PATH="/usr/local/lib"