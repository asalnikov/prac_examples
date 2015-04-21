FLAGS= -g

all: server client

server: server.c
	gcc $(FLAGS) -o $@ $^ -lpthread

client: client.c
	gcc $(FLAGS) -o $@ $^

clean:
	rm server client
