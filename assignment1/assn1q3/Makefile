CPP_FILES := $(wildcard *.cpp)
OBJ_FILES := $(CPP_FILES:.cpp=.o)

all: assn1q3

assn1q3: $(OBJ_FILES)
	g++ -fsanitize=address -o $@ $^

%.o: %.cpp
	g++ -std=c++14 -Wall -Werror -O2 -fsanitize=address -c -o $@ $<

clean:
	rm *o assn1q3
