OBJS=argparse.o builtin.o ush.o
HEADERS=argparse.h builtin.h

ush: ${OBJS} ${HEADERS}
	${CC} ${CFLAGS} -o ush ${OBJS} -lm

clean:
	rm *.o ush
