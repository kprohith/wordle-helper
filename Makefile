all: wordle-helper
wordle-helper: wordle.c
	gcc -pedantic -g -Wall -std=gnu99 -I/local/courses/csse2310/include -L/local/courses/csse2310/lib -lcsse2310a1 -o $@ $<
clean:
	rm wordle-helper
