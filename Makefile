CC= gcc
EXE_NAME= a.out
FLAGS= -Wall -g -Wno-unused

build: main.o graphic.o
	$(CC) $(FLAGS) main.o graphic.o -lpanel -lncurses -o $(EXE_NAME)

main.o: main.c graphic.c
	$(CC) $(FLAGS) -c main.c

graphic.o: graphic.h graphic.c
	$(CC) $(FLAGS) -c graphic.c

test: build
	./$(EXE_NAME)

clean:
	rm *.o $(EXE_NAME)
