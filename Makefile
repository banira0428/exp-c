NAME =

compile:
	gcc-10 -o out/${NAME}.out src/${NAME}.c

run: 
	gcc-10 -o out/${NAME}.out src/${NAME}.c
	out/${NAME}.out data/${NAME}