mshell:
	gcc main.c wildcard.c inlinemath.c -o bin/mshell -lm
debug:
	gcc -g main.c wildcard.c -o bin/debug
