	/*** INCLUDES ***/

#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>

	/***DEFINES***/
#define CTRL_KEY(k) ((k) & 0x1f)
#define KUL_VERSION "0.2.0"
#define ABUF_INIT {NULL, 0}

enum editor_key{
	ARROW_LEFT = 1000,
	ARROW_RIGHT,
	ARROW_UP, 
	ARROW_DOWN,
	DELETE_KEY,
	HOME_KEY,
	END_KEY,
	PAGE_UP,
	PAGE_DOWN 
};

	/*** DATA ***/

typedef struct erow{
	int size;
	char *chars;
} erow;

struct config
{
	int cx, cy; /*Координаты курсора*/
	int screenrows;
	int screencols;
	int num_row;
	erow *row;
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
	term.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	term.c_oflag &= ~(OPOST);
	term.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG); /*ICANON — включает стандартную обработку ввода*/
	term.c_cc[VMIN] = 0;
	term.c_cc[VTIME] = 1;

	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) == -1)/*TCALFLUSH изменяет значения, когда текущий вывод завершен, но отбрасывает любой ввод, доступный в текущий момент и все еще не возвращенный вызовом read.*/
		die("tcsetattr");
}	

int read_key()
{
	int nread;
	char c;
	while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    	if (nread == -1 && errno != EAGAIN) 
    		die("read");
  	}

  	if (c == '\x1b'){
  		char seq[3];
  	

  	if(read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
  	if(read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';

  	if(seq[0] == '['){
  		if (seq[1] >= '0' && seq[1] <='9'){
  			if (read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b';
  			if (seq[2] == '~'){
  				switch (seq[1]){
  					case '1': return HOME_KEY;
  					case '2': return END_KEY;
  					case '3': return DELETE_KEY;
  					case '5': return PAGE_UP;
  					case '6': return PAGE_DOWN;
  					case '7': return HOME_KEY;
  					case '8': return END_KEY;
  				}
  			}
  		} else{
  			switch (seq[1]){
  				case 'A' : return ARROW_UP;
  				case 'B' : return ARROW_UP;
  				case 'C' : return ARROW_UP;
  				case 'D' : return ARROW_LEFT;
  				case 'H' : return HOME_KEY;
  				case 'F' : return END_KEY;
  			}
  		  }
	}	else if(seq[0] == '0'){
		switch(seq[1]){
			case 'H': return HOME_KEY;
			case 'F': return END_KEY;
		}
	}
  	return '\x1b';
    } else {
  		return c;
      }	
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
	/***ROW OPERATIONS***/

void append_row(char *s, size_t len)
{	
	CONFIG.row = realloc(CONFIG.row, sizeof(erow) * (CONFIG.num_row + 1));

	int at = CONFIG.num_row;
	CONFIG.row[at].size = len;
	CONFIG.row[at].chars = malloc(len + 1);
	memcpy(CONFIG.row[at].chars, s, len);
	CONFIG.row[at].chars[len] = '\0';
	CONFIG.num_row++;
	
}

	/***FILE I/O***/

void editor_open(char *file_name)
{
	FILE *fp = fopen(file_name, "r");
	if(!fp) die("fopen");

	char *line = NULL;
	size_t linecap = 0;
	ssize_t line_length;
	while ((line_length = getline(&line, &linecap, fp)) != -1){
		while (line_length > 0 && (line[line_length - 1] == '\n' ||
									line[line_length - 1] == '\r'))
			line_length--;
		append_row(line, line_length);
	}
	free(line);
	free(fp);
}

	/***APPEND BUFFER***/

struct append_buffer { /*Структора динамической строки*/
	char *b;
	int len; 
};

void ab_append(struct append_buffer *ab, const char *s, int len)
{
	char *new = realloc(ab->b, ab ->len + len);

	if (new == NULL) return;

	memcpy(&new[ab->len], s, len); /*копирую строку s после окончания текущих данных в буфере,
	и обновляю указатель и длину abuf до новых значений.*/
	ab->b = new;
	ab->len += len;
}

void ab_free(struct append_buffer *ab)
{
	free(ab->b);/*освобождаю память*/
}

	/*** OUTPUT ***/

void drawRows(struct append_buffer *ab)
{
	int i;
	for (i = 0; i < CONFIG.screenrows; ++i)
	{	
		if(i >= CONFIG.num_row){
			if (CONFIG.num_row == 0 &&i == CONFIG.screenrows / 3) {
				char welcome[80];
				int welcome_len = snprintf(welcome, sizeof(welcome),
					"KulEditor -- version %s", KUL_VERSION);
				if (welcome_len > CONFIG.screencols) welcome_len = CONFIG.screencols;
				int pad = (CONFIG.screencols - welcome_len) / 2;
				if(pad){
					ab_append(ab, "~", 1);
					pad--;
				}
				while (pad--) ab_append(ab, " ", 1);
				ab_append(ab, welcome, welcome_len);
			} else{
			ab_append(ab, "~", 1); /*Заменить потом на цифры!*/
			}
		} else {
			int len = CONFIG.row[i].size;
			if(len > CONFIG.screencols) len = CONFIG.screencols; /*Вот тут я обрезал строку(добавить потом режим переноса?)*/
			ab_append(ab, CONFIG.row[i].chars, len);
		}

		ab_append(ab, "\x1b[K", 3);
		if (i < CONFIG.screenrows - 1)
		{
			ab_append(ab, "\r\n", 2);
		}
	}
}

void refresh_screen()
{
	struct append_buffer ab = ABUF_INIT;
	ab_append(&ab, "\x1b[?25l", 6);
	ab_append(&ab, "\x1b[H", 3);

	drawRows(&ab);

	char buf[32];
	snprintf(buf, sizeof(buf), "\x1b[%d;%dH", CONFIG.cy + 1, CONFIG.cx + 1);/*Точное положение курсора*/
	ab_append(&ab, buf, strlen(buf));

	ab_append(&ab, "\x1b[?25h", 6);

	write(STDOUT_FILENO, ab.b, ab.len);
	ab_free(&ab);
}

	/*** INPUT ***/
void move_cursor(int key)
{
	switch(key){

		case ARROW_LEFT:
		if(CONFIG.cx != 0) /*Прверка на границы экрана*/
    		CONFIG.cx--;
       	break;

    	case ARROW_RIGHT:
    	if(CONFIG.cx != CONFIG.screencols - 1)
      		CONFIG.cx++;
        break;
 
        case ARROW_UP:
        if(CONFIG.cy != 0)
        	CONFIG.cy--;
        break;

        case ARROW_DOWN:
        if(CONFIG.cy != CONFIG.screenrows - 1)
        	CONFIG.cy++;
        break;
	}
}

void keypress()
{
	int c = read_key();

	switch(c){
		case CTRL_KEY('q'):
		write(STDOUT_FILENO, "\x1b[2J", 4);
		write(STDOUT_FILENO, "\x1b[H", 3);
		exit(0);
		break;

		case HOME_KEY:
		CONFIG.cx = 0;
		break;
		case END_KEY:
		CONFIG.cx = CONFIG.screencols - 1;
		break;

		case PAGE_UP:
		case PAGE_DOWN:
		{
			int times = CONFIG.screenrows;
			while(times --)
				move_cursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
		}
		break;

		case ARROW_UP:
		case ARROW_DOWN:
		case ARROW_LEFT:
		case ARROW_RIGHT:
			move_cursor(c);
			break;
	}
}

	/*** INIT ***/

void init()
{
	CONFIG.cx = 0;
	CONFIG.cy = 0;
	CONFIG.num_row = 0;
	CONFIG.row = NULL;

	if(get_window_size(&CONFIG.screenrows, &CONFIG.screencols) == -1)
		die("get_window_size");
}

int main(int argc, char *argv[]) 
{
	raw_mode_on();
	init();
	if(argc >= 2){
		editor_open(argv[1]);
	}

	while (1){
		refresh_screen();
		keypress();
	}
  	return 0;
}