	/*** INCLUDES ***/

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>

/*** DATA ***/

struct termios global_term; /*Original terminal*/

/*** TERMINAL ***/

void die(const char *s)
{
	perror(s);
	exit(1);
}

void raw_mode_off()
{
	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &global_term) == -1)
		die("tcsetattr");
}

void raw_mode_on() /*Включаем интерфейс терминала*/
{
	if(tcgetattr(STDIN_FILENO, &global_term) == -1)/*Записал текущие значения переменных интерфейса*/
		die("tcgetattr");

	atexit(raw_mode_off); /*сохраняем вид терминала*/

	struct termios term = global_term;
	term.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);\
	term.c_oflag &= ~(OPOST);
	term.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG); /*ICANON — включает стандартную обработку ввода*/
	term.c_cc[VMIN] = 0;
	term.c_cc[VTIME] = 1;

	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) == -1)/*TCALFLUSH изменяет значения, когда текущий вывод завершен, но отбрасывает любой ввод, доступный в текущий момент и все еще не возвращенный вызовом read.*/
		die("tcsetattr");
}	

/*** INIT ***/

int main() 
{
	raw_mode_on();

	while (1){
		char c = '\0';
		if(read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN)
			die("read");
		if (iscntrl(c)){ /*Проверка на управляющий символ*/
			printf("%d\n", c);
		}else{
			printf("%d ('%c')\r\n", c, c);
		}
		if(c == 'q') 
			break;
	}
  	return 0;

}