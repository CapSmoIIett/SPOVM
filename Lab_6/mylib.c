//#include "mylib.h"

#include <unistd.h> 

int has_initialized = 0;
void *managed_memory_start;
void *last_valid_address;


struct mem_control_block {
    int is_available;
    int size;
};

void malloc_init()
{
    last_valid_address = sbrk(0);               // захватить (запросить у системы) последний валидный адрес 

    managed_memory_start = last_valid_address;  //пока у нас нет памяти, которой можно было бы управлять
	                                            // установим начальный указатель на last_valid_address

    has_initialized = 1;                        // Инициализация прошла, теперь можно пользоваться */
 	
}

void free(void *firstbyte) {
	struct mem_control_block *mcb;

	mcb = firstbyte - sizeof(struct mem_control_block); // Отматываем текущий указатель и работаем с ним как с mem_control_block
	
	mcb->is_available = 1;                      // Помечаем блок как доступный 

	return;
}

void *malloc(long numbytes) {
	
	void *current_location;                     // Место откуда начинается поиск
	
	struct mem_control_block *current_location_mcb; // Представим что мы работаем с memory_control_block

	void *memory_location;                      // В этот указатель мы вернём найденную память. На время поиска он должен быть 0 

	// Инициализируем, если мы этого не сделали 
	if(! has_initialized) 	{
		malloc_init();
	}

	// Память содержит в себе memory
	// control block, но пользователям функции mallocне нужно
	// об этом знать. Просто смещаем указатель на размер структуры
	numbytes = numbytes + sizeof(struct mem_control_block);

	memory_location = 0;                        // Присваиваем memory_location 0 пока не найдем подходящий участок 
	

	current_location = managed_memory_start;    // Начинаем поиск с начала доступной (управляемой) памяти */
	
	  
	while(current_location != last_valid_address)   // Ищем по всему доступному пространству
	{
		// По факту current_location и current_location_mcb
		// одинаковые адреса.  Но current_location_mcb
		// мы используем как структуру , а 
		// current_location как указатель для перемещенияt
		 
		current_location_mcb = (struct mem_control_block *)current_location;

		if(current_location_mcb->is_available)
		{
			if(current_location_mcb->size >= numbytes)
			{
				// Воооу! Мы нашли подходящий блок... 

				// Кто первым встал, того и тапки - отмечаем участок как занятый 
				current_location_mcb->is_available = 0;

				// Мы оккупировали эту территорию 
				memory_location = current_location;

				break;
			}
		}

		// Если мы оказались здесь, это потому что текущиё блок памяти нам не подошёл, сяпаем дальше 
		current_location = current_location + current_location_mcb->size;
	}

	// Если мы всё ещё не имеем подходящего адреса, то следует запросить память у ОС 
	if(! memory_location)
	{
		// Move the program break numbytes further 
		sbrk(numbytes);

		// После выделения, last_valid_address должен обновится 
		memory_location = last_valid_address;

		// Перемещаемся от last valid address на
		// numbytes вперёд
		 
		last_valid_address = last_valid_address + numbytes;

		// И инициализируем mem_control_block 
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