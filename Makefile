make:
	gcc -c neocities.c -o neocities.o
	ar rcs libneocities.a neocities.o

clean:
	rm libneocities.a neocities.o

example:
	rm -f example
	gcc example.c -L. -lneocities -lcurl -o example
