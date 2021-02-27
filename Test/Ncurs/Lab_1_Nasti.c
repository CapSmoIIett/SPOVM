#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <sys/types.h>
#include <time.h>

int main ()
{
    initscr();                      // Включает режим работы с библиотекой
    if(!has_colors()){printf("(");return -1;}   // Проверка, может ли консоль выводить цвета
    raw();                          //  Символы вводимые с клавиатуры не буферизируются, а сразу вносятся в программу 
    start_color();                  //
    init_pair(1, COLOR_MAGENTA, COLOR_BLACK);   // Иницилируем комбинацию, черный цвет на розовом фоне
    attron(COLOR_PAIR(1) | A_STANDOUT | A_UNDERLINE);  // Устанавливает пару цветов


    long int ttime;
    ttime = time(NULL);             // Берем текущее время

    pid_t pid = fork();              
    if (pid == 0){
        printw("Child    %4d\n", getpid());      //PRINTW - вывод для библиотеки
        printw("Now  %4s\n", ctime(&ttime));            
    }
    else {
        printw("Parent   %4d\n", getpid());
        printw("Now  %4s\n", ctime(&ttime));
    }

   // mvprintw(5, 10, "Something");   // вывод по корлинатоам

    attroff(COLOR_PAIR(1) | A_STANDOUT | A_UNDERLINE);  // отКЛЮЧАЕМ ЦВЕТОВУЮ ПАРУ
    wait();
    //fflush(stdin);   
    refresh();	
    halfdelay(10);		// Ждет ввода в течении некоторого времени, после продолжает работу
    	getch();		
	endwin();	
    return 0;
}