CC = g++
CFLAGS = -g -Wall -lrt -pthread -fopenmp
LDFLAGS =

# Source files
LAB2_SRCS = knn.cpp
LAB2_SRCS1 = knnImageClass.cpp

all: knn knnImageClassification

knn: $(LAB2_SRCS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(LAB2_SRCS)

knnImageClassification: $(LAB2_SRCS1)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(LAB2_SRCS1)

clean:
	rm -f knn knnImageClassification
