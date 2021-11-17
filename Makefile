CC=gcc
CFLAGS=-Wall -std=c11 -pedantic -lm
FILES=scanner.c tests/scanner_token_test.c

.PHONY: build clean run

build: $(FILES)
	$(CC) $(CFLAGS) -g -o $@ $(FILES)

test:
	./build < tests/scanner_token_test_input.txt > scanner_token_test.output

clean:
	rm -f build
