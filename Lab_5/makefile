all:
	clear
	gcc -fPIC -c mylib.c 
	# -f - это префикс gcc для опций. PIC расшифровывается как "позиционно-независимый код"
	gcc -shared  -nostartfiles -o lmylib.so mylib.o -lrt
	#когда не хотите, чтобы какие-либо стандартные библиотеки (например, libc и libgcc) были 
	#связаны и выполнялись стандартные действия инициализации, такие как вызов конструкторов C ++
	gcc -g -c main.c 
	gcc -g -rdynamic main.o  -o main -ldl -lpthread

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
	

	
