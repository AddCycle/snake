all: build run

build:
	clang snake.c -o snake \
	-I "C:/SDL3/include" -L "C:/SDL3/lib/x64" -lSDL3

run:
	./snake

clean:
	rm -rf *.o snake