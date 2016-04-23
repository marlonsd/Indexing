# UNAME_S := $(shell uname -s)
# TOP := $(shell pwd)
# ifeq ($(UNAME_S),Linux)
#     FLAGS += $(TOP)/chilkat/lib/libchilkat-9.5.0.so
# endif
# ifeq ($(UNAME_S),Darwin)
#     FLAGS += chilkat/lib/libchilkat.a
# endif

FLAGS += -I /usr/local/include/htmlcxx -L/usr/local/lib -lhtmlcxx

crawler: main.o tokenizer.o
	g++ -std=c++11 tokenizer.o main.o $(FLAGS) -o indexing

main.o: main.cpp tokenizer.h
	g++ -std=c++11 $(FLAGS) -c main.cpp

tokenizer.o: tokenizer.cpp tokenizer.h
	g++ -std=c++11 -c tokenizer.cpp	

clean:
	rm *.o indexing

# dir:
# 	mkdir htmls logs backup cache