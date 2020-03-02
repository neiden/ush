output: argparse.o builtin.o ush.o
	gcc argparse.o builtin.o ush.o -o output -lm

ush.o: ush.c argparse.h builtin.h  
	gcc -c ush.c -lm

argparse.o: argparse.c argparse.h
	gcc -c argparse.c -lm

builtin.o: builtin.c builtin.h
	gcc -c builtin.c -lm

clean:
	rm *.o output
