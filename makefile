all: shellwiki
CFLAGS = -g
LIBS = -lcjson -lcurl -lcunit
MAINOBJ = main.o 
OBJ= parser.o util.o
TESTOBJ = test_main.o
OUTPUT = shellwiki
TESTOUTPUT = shellwiki_test
SOURCEDIR = ./src
BUILDDIR = ./build
TESTDIR = ./tests

all: shellwiki tests

shellwiki: $(OBJ) $(MAINOBJ) 
	gcc $(CFLAGS) $(MAINOBJ) $(OBJ) -o $(BUILDDIR)/$(OUTPUT) $(LIBS)

tests: shellwiki $(TESTOBJ) 
	gcc $(CFLAGS) $(TESTOBJ) $(OBJ) -o $(TESTDIR)/$(TESTOUTPUT) $(LIBS)

test_main.o: $(TESTDIR)/test_main.c
	gcc $(CFLAGS) -c $(TESTDIR)/test_main.c $(LIBS)

main.o: $(SOURCEDIR)/main.c
	gcc $(CFLAGS) -c $(SOURCEDIR)/main.c $(LIBS)

parser.o: $(SOURCEDIR)/parser.c
	gcc $(CFLAGS) -c $(SOURCEDIR)/parser.c $(LIBS)

util.o: $(SOURCEDIR)/util.c
	gcc $(CFLAGS) -c $(SOURCEDIR)/util.c $(LIBS)

clean: 
	rm -rf *.o $(BUILDDIR)/$(OUTPUT)
