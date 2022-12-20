all: 
	gcc -g -Og -Wall -Wextra src/*.c -Iinclude/ -I../salamander/include -o ../bin/main -lsalamander -L../bin -Wl,-rpath,.