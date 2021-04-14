# SPOVM

## Lab_1: Понятие процесса
Создать прилодение, в котором базовый процесс порождает дочерний. Для вывода использовать библиотеку
ncurses. Добавлен вывод текущего времени.

## Lab_2: Синхронизация процессов
Разработать приложение, главный процесс которого создает дочеринии при нажатии '+', '-' - удаляет 
последний процес, 'q' - завершает программу.

## Lab_3: Взаимодействие процессов
Создание двух приложений, серверного и клиентского. 
- Клиентскиц процес ждет сигнла, что сервер готов начать передачу данных
- Серверный процесс получает от пользователя строку и передает ее клиенту используя разделяемую память
- Клиент принимает строку и выводит ее на экран
- Сервер ожидает уведомления от клинента о успешном получении строки
- Сервер вновь ожидает ввода строки 

## Lab_4: Работа с потоками
Задание аналогично Lab_2 но с реализацией с помощью потоков 

## Lab_5: Асинхронные файловые операции. Динамические библиотеки
- Cсделана динамическая библиотка, читающая файлы через aio. (write сдметром сделан с параметром "a")
- Есть потоки чтения и записи. 
- Поток чтения сам просматривает директорию в поисках *.txt файлов.
- Поток записи удаляет файл outfile.txt (напомню что write не удаляет существующий файл, а дописывает 
его)
- Передача данных между потоков реализованна с помощью общей памяти.

## Lab_6: Разработка менеджера памяти 
Разработать менеджер памяти, реализующий malloc и free. Дополнительное задание (по варианту): 
динамическое изменение размеров выделенной области (realloc)
