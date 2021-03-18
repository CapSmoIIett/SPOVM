all:
	clear
	gcc -E 	mylib.c  -o 	mylib.ii 		# Препроцессинг
	gcc -S 	mylib.ii -o 	mylib.s 		# Компиляция
	rm		mylib.ii
	as     	mylib.s  -o 	mylib.o		# Ассемблирование
	rm     	mylib.s
	gcc -g -shared	mylib.c  -o 	main			# Компоновка
	
	./ main
