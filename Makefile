NAME =

compile:
	gcc-10 -o out/${NAME}.out src/${NAME}.c

run: 
	gcc-10 -o out/${NAME}.out src/${NAME}.c
	out/${NAME}.out data/${NAME}

client: 
	gcc-10 -o out/meibo_client.out src/meibo_client.c src/commands.c

server: 
	gcc-10 -o out/meibo_server.out src/meibo_server.c