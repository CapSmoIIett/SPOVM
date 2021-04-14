all:
	clear
	gcc -shared -fpic mylib.c -o mylib.so -lrt
	gcc -o main main.c -ldl #mylib.so 
	./main
