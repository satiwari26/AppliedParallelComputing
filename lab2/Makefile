CC = g++
CFLAGS = -g -Wall -lrt -pthread -fopenmp
LDFLAGS =

# Source files
LAB2_SRCS = knn.cpp

all: knn

knn: $(LAB2_SRCS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(LAB2_SRCS)

clean:
	rm -f knn
