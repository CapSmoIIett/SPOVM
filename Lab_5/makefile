all:
	clear
	gcc -fPIC -c mylib.c 
	gcc -shared  -nostartfiles -o lmylib.so mylib.o
	gcc -c main.c
	gcc -rdynamic main.o  -o main -ldl

	./main 
	
	
	

2:
	clear
	gcc -E 	mylib.c  	-o 	mylib.ii 		# Препроцессинг
	gcc -S 	mylib.ii 	-o 	mylib.s 		# Компиляция
	rm		mylib.ii
	as     	mylib.s  	-o 	mylib.o		# Ассемблирование
	rm     	mylib.s
	gcc -shared 	mylib.o	-o 	lmylib.so
	
	
	gcc -E 	main.c  	-o 	main.ii 		# Препроцессинг
	gcc -S 	main.ii 	-o 	main.s 		# Компиляция
	rm		main.ii
	as     	main.s  	-o 	main.o			# Ассемблирование
	rm     	main.s
	
	
	gcc  main.o -L lmylib	-o 	main			# Компоновка
	rm main.o
	rm mylib.o
	
	./main
	

	
