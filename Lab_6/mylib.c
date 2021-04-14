#include <unistd.h> 
#include <stdio.h>

//#define debug

/*
	Сборщика мусора не будет 
	т.к. для него необходим C++-шный деструктор,
	а я хочу все сделать на C. 
	Сборщик просто проходился бы по все выделенной памяти и обнулял ее,
	а после освобождала ее (аналогично дефрагментатору)
*/


int has_initialized = 0;
void *managed_memory_start;
void *last_valid_address;		// место break_point-a

struct mem_control_block {
    int is_available;				// Доступна ли память
    int size;						// Размер памяти
};

void malloc_init()
{
	#ifdef debug
		printf("Init\n");
	#endif
    last_valid_address = sbrk(0);               // Ищем текущее положение break_point

	/*if ((int)last_valid_address == -1){			// Проверка
		printf("Init error\n");
	}*/
    managed_memory_start = last_valid_address;  //пока у нас нет памяти, которой можно было бы управлять
	                                            // установим начальный указатель на last_valid_address

    has_initialized = 1;                        // Инициализация прошла, теперь можно пользоваться */
}

void free(void *firstbyte) {

	#ifdef debug
		printf("free\n");
	#endif

	struct mem_control_block *mcb;

	mcb = firstbyte - sizeof(struct mem_control_block); // Отматываем текущий указатель и работаем с ним как с mem_control_block
	
	mcb->is_available = 1;                      // Помечаем блок как доступный 

	return;
}


/*
	malloc - ищет подходящий, уже выделенный кусок памяти, если такой нет -
	смещает break_point  (увеличивает пространство данных программы) на необходимую память + 
	структура с параметрами этого куска памяти
*/



void *malloc(long numbytes) {
	#ifdef debug
		printf("Malloc\n");
	#endif
	
	void *current_location;                     // Место откуда начинается поиск
	
	struct mem_control_block *current_location_mcb; // Создаем memory_control_block, с данными о выделенной памяти

	void *memory_location;                      // В этот указатель мы вернём найденную память. На время поиска он должен быть 0 

	// Инициализируем, если мы этого не сделали 
	if(! has_initialized) 	{
		malloc_init();
	}

	// Каждый выделенный фрагмент памяти содержит в себе mem_control_block
	// чтобы пользователю е нужно было это учитывать, мы сами добавляем размер этой структуры
	numbytes = numbytes + sizeof(struct mem_control_block);

	memory_location = 0;                        // Присваиваем memory_location 0 пока не найдем подходящий участок 
	
	current_location = managed_memory_start;    // Начинаем поиск с начала доступной (управляемой) памяти
	
	  
	while(current_location != last_valid_address)   // Ищем по всему доступному пространству
	{
		// current_location и current_location_mcb
		// одинаковые адреса.  Но current_location_mcb
		// мы используем как структуру , а 
		// current_location как указатель для перемещения
		 
		current_location_mcb = (struct mem_control_block *)current_location; 	// То есть мы сможем обратиться к mem_control_block 
																				// текущего фрагмента памяти

		if(current_location_mcb->is_available)				// Если память свободна
		{
			if(current_location_mcb->size >= numbytes)		// И если она подходит по размеру
			{
				#ifdef debug
					printf("found block\n");
				#endif
	
				current_location_mcb->is_available = 0;		// Отмечаем участок как занятый
				memory_location = current_location;			// и запоминаем его адресс

				break;
			}
		}

		current_location = current_location + current_location_mcb->size; // переходим на следующий фрагмент памяти
	}

	// Если мы всё ещё не имеем подходящего адреса, то следует запросить память у ОС 
	if(! memory_location)
	{
		#ifdef debug
			printf("allocate block\n");
		#endif

		sbrk(numbytes);		// Сдвигаем break_point 

		memory_location = last_valid_address;		// Last valid adress указывает на начало выделенной памяти
		 
		last_valid_address = last_valid_address + numbytes;	// перемещаем lva на размер выделенной памяти

		// И инициализируем mem_control_block (он располагается в начале выделленой памяти)
		current_location_mcb = memory_location;
		current_location_mcb->is_available = 0;
		current_location_mcb->size = numbytes;

	}

	// Теперь мы получили память (если не получили ошибок). 
	// И в memory_location также есть место под
	// mem_control_block
	 

	// Перемещаем указатель в конец mem_control_block 
	memory_location = memory_location + sizeof(struct mem_control_block);

	// Возвращаем указатель s
	return memory_location;
 }

 void *calloc(long num, long size) {		// такие параметры выбранны что бы убрать warning
	
	#ifdef debug
			printf("Calloc\n");
	#endif
	
	void *current_location;                     // Место откуда начинается поиск
	unsigned char *new_pointer;
	
	struct mem_control_block *current_location_mcb; // Создаем memory_control_block, с данными о выделенной памяти

	void *memory_location;                      // В этот указатель мы вернём найденную память. На время поиска он должен быть 0 

	// Инициализируем, если мы этого не сделали 
	if(! has_initialized) 	{
		malloc_init();
	}

	// Каждый выделенный фрагмент памяти содержит в себе mem_control_block
	// чтобы пользователю е нужно было это учитывать, мы сами добавляем размер этой структуры
	long numbytes = size * num + sizeof(struct mem_control_block);

	memory_location = 0;                        // Присваиваем memory_location 0 пока не найдем подходящий участок 
	
	current_location = managed_memory_start;    // Начинаем поиск с начала доступной (управляемой) памяти
	
	  
	while(current_location != last_valid_address)   // Ищем по всему доступному пространству
	{
		current_location_mcb = (struct mem_control_block *)current_location; 	// То есть мы сможем обратиться к mem_control_block 
																				// текущего фрагмента памяти

		if(current_location_mcb->is_available)				// Если память свободна
		{
			if(current_location_mcb->size >= numbytes)		// И если она подходит по размеру
			{
					#ifdef debug
						printf("found block\n");
					#endif
		
				current_location_mcb->is_available = 0;		// Отмечаем участок как занятый
				memory_location = current_location;			// и запоминаем его адресс

				break;
			}
		}

		current_location = current_location + current_location_mcb->size; // переходим на следующий фрагмент памяти
	}

	// Если мы всё ещё не имеем подходящего адреса, то следует запросить память у ОС 
	if(! memory_location)
	{

		#ifdef debug
			printf("allocate block\n");
		#endif
		

		sbrk(numbytes);		// Сдвигаем break_point 

		memory_location = last_valid_address;		// Last valid adress указывает на начало выделенной памяти
		 
		last_valid_address = last_valid_address + numbytes;	// перемещаем lva на размер выделенной памяти

		// И инициализируем mem_control_block (он располагается в начале выделленой памяти)
		current_location_mcb = memory_location;
		current_location_mcb->is_available = 0;
		current_location_mcb->size = numbytes;

	}

	// Перемещаем указатель в конец mem_control_block 
	memory_location = memory_location + sizeof(struct mem_control_block);
	new_pointer = memory_location;

	for (int i = 0; i < numbytes; i++){
		new_pointer[i] = 0;
	}

	// Возвращаем указатель s
	return memory_location;
 }



void* realloc(void* pointer, long newnumbytes){

	#ifdef debug
		printf("Realloc\n");
	#endif
	void *current_location;                     // Место откуда начинается поиск
		
	
	unsigned char *old_pointer = pointer;		// указатель на старую память ( для копирования)
	unsigned char *new_pointer;					// указатель на новую память (для копирования)
	long old_size = ((struct mem_control_block*)(pointer - sizeof(struct mem_control_block)))->size; 	// получаем размер уже имеющейся памяти
	long new_size = newnumbytes;
	//printf("%d %d\n", len1 / sizeof(int), newnumbytes  / sizeof(int));
	//len1 = (len1 > newnumbytes) ? newnumbytes : len1;			// выбираем наименьшую длинну 

	struct mem_control_block *current_location_mcb; // Создаем memory_control_block, с данными о выделенной памяти

	void *memory_location;                      // В этот указатель мы вернём найденную память. На время поиска он должен быть 0 

	// Инициализируем, если мы этого не сделали 
	if(! has_initialized) 	{					// Вдруг пользователь идио..., новичок
		malloc_init();
	}

	newnumbytes = newnumbytes + sizeof(struct mem_control_block);	// учитываем структуру данных

	memory_location = 0;                        // Присваиваем memory_location 0 пока не найдем подходящий участок 
	
	current_location = managed_memory_start;    // Начинаем поиск с начала доступной (управляемой) памяти
	
	while(current_location != last_valid_address)   // Ищем по всему доступному пространству
	{
		// current_location и current_location_mcb
		// одинаковые адреса.  Но current_location_mcb
		// мы используем как структуру , а 
		// current_location как указатель для перемещения
		 
		current_location_mcb = (struct mem_control_block *)current_location; 	// То есть мы сможем обратиться к mem_control_block 
																				// текущего фрагмента памяти

		if(current_location_mcb->is_available)				// Если память свободна
		{
			if(current_location_mcb->size >= newnumbytes)		// И если она подходит по размеру
			{
				#ifdef debug
					printf("found block\n");
				#endif
				
				current_location_mcb->is_available = 0;		// Отмечаем участок как занятый
				memory_location = current_location;			// и запоминаем его адресс

				break;
			}
		}

		current_location = current_location + current_location_mcb->size; // переходим на следующий фрагмент памяти
	}

	if(! memory_location)	// Создаем участок памяти под нас
	{
		#ifdef debug
			printf("allocate block\n");
		#endif
		

		sbrk(newnumbytes);		// Сдвигаем break_point 

		memory_location = last_valid_address;		// Last valid adress указывает на начало выделенной памяти
		 
		last_valid_address = last_valid_address + newnumbytes;	// перемещаем lva на размер выделенной памяти

		// И инициализируем mem_control_block (он располагается в начале выделленой памяти)
		current_location_mcb = memory_location;
		current_location_mcb->is_available = 0;
		current_location_mcb->size = newnumbytes;

	}

	

	// Перемещаем указатель в конец mem_control_block 
	memory_location = memory_location + sizeof(struct mem_control_block);
	new_pointer = memory_location;

	
	if (old_size < new_size){
		int i = 0;
		for (; i < old_size; i++){
			new_pointer[i] = old_pointer[i];	// переносим значения
		}
		for (; i < new_size; i++){				// Мы могли взять кже существующик кусок памяти
			new_pointer[i] = 0;					// поэтому мы должны очистить хвост
		}
	}
	else {
		int i = 0;
		for (; i < new_size; i++){
			new_pointer[i] = old_pointer[i];	// Переносим значения
		}
		for (; i < old_size; i++){
			new_pointer[i] = 0;
		}
	}
		
	

	
	pointer = 0;		// обнуляе  прошлый указатель, чтобы не трогали нашу памяти
	// Возвращаем указатель s
	return memory_location;

}

void defragmentation(){
/*
	ищем все не занятые участки памяти
*/

}
