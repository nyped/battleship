CC= gcc
EXE_NAME= a.out
CFLAGS= -Wall -g 

build: main.o graphic.o core.o populate.o bot.o
	$(CC) $(CFLAGS) main.o graphic.o core.o populate.o bot.o -lncurses -o $(EXE_NAME)

main.o: main.c graphic.c
	$(CC) $(CFLAGS) -c main.c

graphic.o: graphic.h graphic.c
	$(CC) $(CFLAGS) -c graphic.c

core.o: core.c core.h
	$(CC) $(CFLAGS) -c core.c

populate.o: populate.c populate.h
	$(CC) $(CFLAGS) -c populate.c

bot.o: bot.c bot.h
	$(CC) $(CFLAGS) -c bot.c

test: build
	./$(EXE_NAME)

clean:
	rm *.o $(EXE_NAME)
