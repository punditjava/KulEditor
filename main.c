	/*** INCLUDES ***/

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>

	/***DEFINES***/
#define CTRL_KEY(k) ((k) & 0x1f)

	/*** DATA ***/
struct config
{
	int screenrows;
	int screencols;
	struct termios global_term; /*Original terminal*/	
};

struct config CONFIG; /*Глобальная структура(состояяние редактора)*/

	/*** TERMINAL ***/

void die(const char *s)
{
	write(STDOUT_FILENO, "\x1b[2J", 4);/*\x1b[ - escape последовательность(супер полезно и важно!!!)*/
	write(STDOUT_FILENO, "\x1b[H", 3);/*2J-стёрли экран, H - курсор*/

	perror(s);
	exit(1);
}

void raw_mode_off()
{
	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &CONFIG.global_term) == -1)
		die("tcsetattr");
}

void raw_mode_on() /*Включаем интерфейс терминала*/
{
	if(tcgetattr(STDIN_FILENO, &CONFIG.global_term) == -1)/*Записал текущие значения переменных интерфейса*/
		die("tcgetattr");

	atexit(raw_mode_off); /*сохраняем вид терминала*/

	struct termios term = CONFIG.global_term;
	term.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);\
	term.c_oflag &= ~(OPOST);
	term.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG); /*ICANON — включает стандартную обработку ввода*/
	term.c_cc[VMIN] = 0;
	term.c_cc[VTIME] = 1;

	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) == -1)/*TCALFLUSH изменяет значения, когда текущий вывод завершен, но отбрасывает любой ввод, доступный в текущий момент и все еще не возвращенный вызовом read.*/
		die("tcsetattr");
}	

char read_key()
{
	int nread;
	char c;
	while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    	if (nread == -1 && errno != EAGAIN) 
    		die("read");
  	}

  	return c;
}

int cursor_position(int *rows, int *cols)
{
	char buf[32];
	unsigned int i = 0;

	if(write(STDOUT_FILENO, "\x1b[6n", 4) != 4)
		return -1;

	while(i < sizeof(buf) - 1){
		if (read(STDIN_FILENO, &buf[i], 1) != 1) break;
		if (buf[i] == 'R') break;
		i++;
	}
	buf[i] = '\0';

	if (buf[0] != '\x1b' || buf[1] != '[') return -1;/*пропускаем 0 элемент чтобы не печатался \x1b*/
	if(sscanf(&buf[2], "%d;%d", rows, cols) != 2) return -1;

	return 0;
}


int get_window_size(int *rows, int *cols)
{
	struct winsize ws;

	if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0){ /*ioctl помещает в ws размеры экрана*/
		if(write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12)
			return -1;
		return cursor_position(rows, cols);
	}else{
		*cols = ws.ws_col;
		*rows = ws.ws_row;
		return 0;
	}
}

	/*** OUTPUT ***/

void drawRows()
{
	int i;
	for (i = 0; i < CONFIG.screenrows; ++i)
	{
		write(STDOUT_FILENO, "~", 1); /*Заменить потом на цифры!*/

		if (i < CONFIG.screenrows - 1)
		{
			write(STDOUT_FILENO, "\r\n", 2);
		}
	}
}

void refresh_screen()
{
	write(STDOUT_FILENO, "\x1b[2J", 4);
	write(STDOUT_FILENO, "\x1b[H", 3);

	drawRows();

	write(STDOUT_FILENO, "\x1b[H", 3);
}

	/*** INPUT ***/

void keypress()
{
	char c = read_key();

	switch(c){
		case CTRL_KEY('q'):
		write(STDOUT_FILENO, "\x1b[2J", 4);
		write(STDOUT_FILENO, "\x1b[H", 3);
		exit(0);
		break;
	}
}

	/*** INIT ***/

void set_size()
{
	if(get_window_size(&CONFIG.screenrows, &CONFIG.screencols) == -1)
		die("get_window_size");
}

int main() 
{
	raw_mode_on();
	set_size();

	while (1){
		refresh_screen();
		keypress();
	}
  	return 0;

}