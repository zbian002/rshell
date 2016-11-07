COMPILE=g++ 

FLAGS=-W -Wall -Werror -ansi -pedantic 

all: rshell

rshell: src/rshell.cpp
        ([ ! -d bin/ ] && mkdir bin) || [ -d bin/ ] 
	$(COMPILE) $(FLAGS) src/rshell.cpp -o bin/rshell

clean: 
	rm -rf *.o
