EXE_NAME=a.out
FLAGS= -Wall -g 

build: main.c
	gcc $(FLAGS) main.c -lncurses -o $(EXE_NAME)

test: build
	./$(EXE_NAME)

clean:
	rm *.out
