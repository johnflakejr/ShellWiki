all: shellwiki
CFLAGS = -g
LIBS = -lcurl
OBJ = main.o parser.o
OUTPUT = shellwiki

shellwiki: $(OBJ) 
	gcc $(CFLAGS) $(OBJ) -o $(OUTPUT) $(LIBS)

main.o: main.c
	gcc $(CFLAGS) -c main.c $(LIBS)

parser.o: parser.c
	gcc $(CFLAGS) -c parser.c $(LIBS)

clean: 
	rm -rf $(OBJ) $(OUTPUT)
