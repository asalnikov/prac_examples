EXECS= bubble bubble_pthread 


COMPILER_OPTIONS= -g
CC=gcc



all: $(EXECS)

file_reader.c: file_reader.h

bubble: bubble.c file_reader.c 
	$(CC) $(COMPILER_OPTIONS) -o $@ $^

bubble_pthread: bubble_pthread.c file_reader.c 
	$(CC) $(COMPILER_OPTIONS) -o $@ $^ -lpthread


clean:
	rm -rf $(EXECS)

