all:
	clear
	gcc -shared -fpic -g mylib.c -o mylib.so -lrt
	gcc -g -o main main.c -ldl #mylib.so 
	./main

c:
	clear
	gcc -g mylib.c -o mylibtest
	./mylibtest	
