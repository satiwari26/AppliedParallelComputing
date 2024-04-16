CC = gcc
CFLAGS = -g -Wall -lrt -pthread -O3
LDFLAGS =

# Source files
NUMCORES_SRCS = NumCores.c
LAB0_SRCS = lab0.c

all: NumCores lab0

NumCores: $(NUMCORES_SRCS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(NUMCORES_SRCS)

lab0: $(LAB0_SRCS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(LAB0_SRCS)

clean:
	rm -f NumCores lab0 sequencial_Method2D.txt parallel_Method2D.txt sequencial_Method1D.txt parallel_Method1D.txt
