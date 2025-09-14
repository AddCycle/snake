FLAGS=-I "C:/SDL3/include" -L "C:/SDL3/lib/x64" -lSDL3 -lSDL3_ttf
DEBUGGER_FLAGS=-g

all: build run

build:
	clang snake.c -o snake $(FLAGS)
	
run:
	./snake

debug:
	clang snake.c -o snake $(FLAGS) $(DEBUGGER_FLAGS)

clean:
	rm -rf *.o *.pdb snake