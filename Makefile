CC=gcc
CFLAGS=-Wall -std=c11 -pedantic -lm
FILES=scanner.c parser.c stack.c symstack.c symtable.c tests/scanner_token_test.c exp_parser.c err_handler.c generator.c

.PHONY: build clean run test_stack test_scanner

build: $(FILES)
	$(CC) $(CFLAGS) -g -o $@ $(FILES)

test_scanner:
	$(CC) $(CFLAGS) -g -o tests/scanner_test tests/scanner_token_test.c scanner.c
	tests/./scanner_test < tests/scanner_token_test_input.txt
test_stack:
	$(CC) $(CFLAGS) -g -o tests/stack_test tests/stack_test.c stack.c
	tests/./stack_test
test_symtable:
	$(CC) $(CFLAGS) -g -o tests/symtable_test tests/symtable_test.c symtable.c
	tests/./symtable_test
test_parser:
	gcc -Wall -std=c11 -pedantic -lm -g -o parser parser.c
clean:
	rm -f build
