CC = gcc
CFLAGS = -g -Wall
LDFLAGS =

# Source files
SRCS = NumCores.c

TARGET = NumCores

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SRCS)

clean:
	rm -f $(TARGET)
