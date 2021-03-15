all: shellwiki
CFLAGS = -lcurl
OBJ = main.o parser.o
OUTPUT = shellwiki

shellwiki: $(OBJ) 
	gcc $(CFLAGS) $(OBJ) -o $(OUTPUT)

main.o: main.c
	gcc -c main.c

parser.o: parser.c
	gcc -c parser.c

clean: 
	rm -rf $(OBJ) $(OUTPUT)
