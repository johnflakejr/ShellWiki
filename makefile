all: shellwiki
CFLAGS = -g
LIBS = -lcjson -lcurl
OBJ = main.o parser.o util.o
OUTPUT = shellwiki
SOURCEDIR = ./src
BUILDDIR = ./build

shellwiki: $(OBJ) 
	gcc $(CFLAGS) $(OBJ) -o $(BUILDDIR)/$(OUTPUT) $(LIBS)
	rm $(OBJ)

main.o: $(SOURCEDIR)/main.c
	gcc $(CFLAGS) -c $(SOURCEDIR)/main.c $(LIBS)

parser.o: $(SOURCEDIR)/parser.c
	gcc $(CFLAGS) -c $(SOURCEDIR)/parser.c $(LIBS)

util.o: $(SOURCEDIR)/util.c
	gcc $(CFLAGS) -c $(SOURCEDIR)/util.c $(LIBS)

clean: 
	rm -rf $(OBJ) $(BUILDDIR)/$(OUTPUT)
