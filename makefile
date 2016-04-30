# UNAME_S := $(shell uname -s)
# TOP := $(shell pwd)
# ifeq ($(UNAME_S),Linux)
#     FLAGS += $(TOP)/chilkat/lib/libchilkat-9.5.0.so
# endif
# ifeq ($(UNAME_S),Darwin)
#     FLAGS += chilkat/lib/libchilkat.a
# endif

FLAGS += -I /usr/local/include/htmlcxx -L/usr/local/lib -lhtmlcxx

crawler: main.o tokenizer.o func.o
	g++ -std=c++11 func.o tokenizer.o main.o $(FLAGS) -o indexing

main.o: main.cpp tokenizer.h func.h
	g++ -std=c++11 $(FLAGS) -c main.cpp

tokenizer.o: tokenizer.cpp tokenizer.h func.h
	g++ -std=c++11 -c tokenizer.cpp

func.o: func.cpp func.h
	g++ -std=c++11 -c func.cpp		

clean:
	rm *.o indexing

# dir:
# 	mkdir htmls logs backup cache