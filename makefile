covert_echo: src/main.c src/server.c src/client.c src/covert_wrappers.c
	gcc -g -Wall -o covert_echo src/main.c src/server.c src/client.c src/covert_wrappers.c
