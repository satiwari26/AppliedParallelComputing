CC = gcc
CFLAGS = -g -Wall -lrt -pthread -fopenmp
LDFLAGS =

# Source files
LAB1_SRCS = gameOfLife.c

all: gameOfLife

gameOfLife: $(LAB1_SRCS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(LAB1_SRCS)

clean:
	rm -f gameOfLife
