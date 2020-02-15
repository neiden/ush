output: argparse.o builtin.o ush.o
	gcc argparse.o builtin.o ush.o -o output

ush.o: ush.c argparse.h builtin.h  
	gcc -c ush.c

argparse.o: argparse.c argparse.h
	gcc -c argparse.c

builtin.0: builtin.c builtin.h
	gcc -c builtin.c

clean:
	rm *.o output
