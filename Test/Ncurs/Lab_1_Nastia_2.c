#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <sys/types.h>
#include <time.h>

int main ()
{
    initscr();
    if(!has_colors()){printf("(");return -1;}
    raw(); // управление клавиатурой
    start_color();
    init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
    attron(COLOR_PAIR(1) | A_STANDOUT | A_UNDERLINE);

//mvprintw(5, 10, "Fuck you");
    
    
//move(3,5);
//chgat(5, A_STANDOUT, 0, NULL);

    long int ttime;
    ttime = time(NULL);

   pid_t pid = fork();
   if (pid == 0){
    printf("Child    %4d\n", getpid());      //PRINTW
    printf("Now  %4s\n", ctime(&ttime));
   }
   else {
    printf("Parent   %4d\n", getpid());
   //long int ttime;
    //ttime = time(NULL);
    printf("Now  %4s\n", ctime(&ttime));}

   // refresh();

    attroff(COLOR_PAIR(1) | A_STANDOUT | A_UNDERLINE);
    wait();
    endwin();
    return 0;
}