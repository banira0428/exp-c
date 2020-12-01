NAME = 1-1
ARG = hoge

run: 
	gcc -o out/${NAME}.out src/${NAME}.c
	out/${NAME}.out ${ARG}