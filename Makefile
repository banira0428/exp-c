NAME =

compile:
	gcc-10 -o out/${NAME}.out src/${NAME}.c

debug:
	gcc-10 -o out/${NAME}.out  -g3 src/${NAME}.c

run: 
	gcc-10 -o out/${NAME}.out src/${NAME}.c
	out/${NAME}.out data/${NAME}

client: 
	gcc-10 -o out/meibo_client.out src/meibo_client.c src/commands.c src/client.c src/process_line.c

cli: 
	out/meibo_client.out

cli-test: 
	out/meibo_client.out < data/input.txt

server: 
	gcc-10 -o out/meibo_server.out src/meibo_server.c src/process_line.c

ser: 
	out/meibo_server.out

meibo: 
	make client
	make server
	out/meibo_server.out &
	out/meibo_client.out

auth:
	gcc-10 -o out/auth_server.out src/auth_server.c src/process_line.c
	gcc-10 -o out/auth_client.out src/auth_client.c src/process_line.c src/client.c
	out/auth_server.out &
	out/auth_client.out


