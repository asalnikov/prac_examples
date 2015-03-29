FLAGS=-g -Wall
BINARIES=messages_one messages_two reader writer

all: $(BINARIES)

messages_one: messages_one.o
	$(CC) -o $@ $^

messages_two: messages_two.o
	$(CC) -o $@ $^

reader: reader.o
	$(CC) -o $@ $^

writer: writer.o
	$(CC) -o $@ $^





%.o: %.c
	$(CC) -c $(FLAGS) $^

clean:
	rm -f $(BINARIES) *.o


