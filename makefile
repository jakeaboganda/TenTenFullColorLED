CC = gcc
CFLAGS = -Wall -DCONFIG_HARDWARE=1 -DDEBUG
DEPS = led0.h
OBJ = led0.o main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

led0.out: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

clean: 
	rm -f *.o *.out
