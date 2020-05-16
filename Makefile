CC= gcc
EXE_NAME= a.out
FLAGS= -Wall -g -Wno-unused

build: main.o graphic.o core.o populate.o
	$(CC) $(FLAGS) main.o graphic.o core.o populate.o -lncurses -o $(EXE_NAME)

main.o: main.c graphic.c
	$(CC) $(FLAGS) -c main.c

graphic.o: graphic.h graphic.c
	$(CC) $(FLAGS) -c graphic.c

core.o: core.c core.h
	$(CC) $(FLAGS) -c core.c

populate.o: populate.c populate.h
	$(CC) $(FLAGS) -c populate.c

test: build
	./$(EXE_NAME)

clean:
	rm *.o $(EXE_NAME)
