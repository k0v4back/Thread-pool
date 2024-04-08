TARGET = thread_pool
CC = g++
GENERAT = $(shell find . -type f -name '*.o') $(shell find . -type f -name '*core.*') $(TARGET)

SRC = $(shell find . -type f -name '*.cpp')
OBJ = $(patsubst %.cpp, %.o, $(SRC))

CFLAGS = -Wall

all: $(TARGET)	

$(TARGET): $(OBJ)
		$(CC) $^ -o $@

%.o: %.cpp
		$(CC) -std=c++14 -g -c $< -lpthread -o $@ $(CFLAGS)

clean:
		rm -f $(GENERAT)