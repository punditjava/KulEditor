	/*** INCLUDES ***/

#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

	/***DEFINES***/
#define CTRL_KEY(k) ((k) & 0x1f)
#define KUL_VERSION "0.3.0"
#define TAB_STOP 8
#define QUIT_TIMES 3
#define ABUF_INIT {NULL, 0}

enum editor_key{
	BACKSPACE = 127,
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
	int rsize;/*размер для вкладок*/
	char *chars;
	char *render;
} erow;

struct config
{
	int cx, cy; /*Координаты курсора*/
	int rx;
	int scroll_ver;
	int scroll_hor;
	int screenrows;
	int screencols;
	int num_row;
	erow *row;
	int dirty;/*проверяем изменён ли файл)*/
	char *file_name;
	char status_message[80];
	time_t status_time;
	struct termios global_term; /*Original terminal*/	
};

struct config CONFIG; /*Глобальная структура(состояяние редактора)*/

	/***PROTOTYPES***/

void set_status_message(const char *fmt, ...);
void refresh_screen();
char *input_prompt(char *prompt);

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
  				case 'B' : return ARROW_DOWN;
  				case 'C' : return ARROW_RIGHT;
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
	char buf[64];
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
	
int row_cx_to_rx(erow *row, int cx)
{
	int rx = 0;
	int j;
	for (j = 0; j < cx; ++j)
	{
		if (row->chars[j] == '\t')
			rx += (TAB_STOP - 1) - (rx % TAB_STOP);
		rx++;
	}
	return rx;
}		

void update_row(erow *row)
{
	int tabs = 0;
	int j;
	for (j = 0; j < row->size; ++j){
		if (row->chars[j] == '\t') tabs++;
	}

	free(row->render);
	row->render = malloc(row->size + tabs*(TAB_STOP - 1) + 1);
	int dx = 0;
	for (j = 0; j < row->size; ++j){
		if (row -> chars[j] == '\t'){
			row -> render[dx++] = ' ';
			while(dx % TAB_STOP != 0) row->render[dx++] = ' ';
		} else {
			row->render[dx++] = row->chars[j];
		}
	}
	row->render[dx] = '\0';
	row->rsize = dx;
}


void insert_row(int at, char *s, size_t len)
{	
	if(at < 0 || at > CONFIG.num_row) return;

	CONFIG.row = realloc(CONFIG.row, sizeof(erow) * (CONFIG.num_row + 1));
	memmove(&CONFIG.row[at + 1], &CONFIG.row[at], sizeof(erow) * (CONFIG.num_row - at));

	CONFIG.row[at].size = len;
	CONFIG.row[at].chars = malloc(len + 1);
	memcpy(CONFIG.row[at].chars, s, len);
	CONFIG.row[at].chars[len] = '\0';

	CONFIG.row[at].rsize = 0;
	CONFIG.row[at].render = NULL;
	update_row(&CONFIG.row[at]);

	CONFIG.num_row++;
	CONFIG.dirty++;
}

void free_row(erow *row) /*Чистим память строки*/
{
	free(row -> render);
	free(row -> chars);
}

void delete_row(int at)
{
	if(at < 0 || at >= CONFIG.num_row) return;
	free_row(&CONFIG.row[at]);
	memmove(&CONFIG.row[at], &CONFIG.row[at + 1], sizeof(erow) * (CONFIG.num_row - at - 1));/*перезаписал удаленную структуру строк с остальными строками, которые идут после нее, и уменьшил num_row. */
	CONFIG.num_row--;
	CONFIG.dirty++;
}

void row_insert_char(erow *row, int at, int c)
{
	if(at < 0 || at > row->size) at = row->size; /*указываем конец строки*/
	row->chars = realloc(row->chars, row->size + 2);
	memmove(&row->chars[at + 1], &row->chars[at], row -> size - at + 1);/*освобождаем место для ещё одного символа*/
	row->size++;
	row->chars[at] = c;/*добавили символ в массив*/
	update_row(row);/*обновили*/
	CONFIG.dirty++;
}

void row_append_string(erow *row, char *s, size_t len)
{
	row->chars = realloc(row->chars, row->size + len + 1);
	memcpy(&row->chars[row->size], s, len);
	row->size += len;
	row->chars[row->size] = '\0';
	update_row(row);
	CONFIG.dirty++;
}

void delete_char(erow *row, int at)
{
	if(at < 0 || at >= row->size) return;
	memmove(&row -> chars[at], &row-> chars[at + 1], row->size - at);
	row->size--;
	update_row(row);
	CONFIG.dirty++;
}


	/*** EDITOR ORERATIONS ***/

void editor_insert_char(int c){
	if (CONFIG.cy == CONFIG.num_row) {/*Если это последняя строка, добавть ещё одну */
		insert_row(CONFIG.num_row, "", 0);
	}
	row_insert_char(&CONFIG.row[CONFIG.cy], CONFIG.cx, c);
}

void editor_insert_new_line()
{
	if(CONFIG.cx == 0){
		insert_row(CONFIG.cy, "", 0);/*просто добавил новую строку*/
	} else {
		erow *row = &CONFIG.row[CONFIG.cy];
		insert_row(CONFIG.cy + 1, &row -> chars[CONFIG.cx], row->size - CONFIG.cx);/*Разбиваем на 2 строки и работает*/
		row = &CONFIG.row[CONFIG.cy];
		row->size = CONFIG.cx;
		row->chars[row->size] = '\0';
		update_row(row);
	}
	CONFIG.cy++;
	CONFIG.cx = 0;
}

void editor_delete_char()
{
	if(CONFIG.cy == CONFIG.num_row) return;
	if(CONFIG.cx == 0 && CONFIG.cy == 0) return;

	erow *row = &CONFIG.row[CONFIG.cy];
	if(CONFIG.cx > 0){
		delete_char(row, CONFIG.cx - 1);
		CONFIG.cx--;
	} else{
		CONFIG.cx = CONFIG.row[CONFIG.cy - 1].size;
		row_append_string(&CONFIG.row[CONFIG.cy - 1], row->chars, row->size);
		delete_row(CONFIG.cy);
		CONFIG.cy--;
	}
}

	/***FILE I/O***/

char *editor_rows_to_string(int *buf_length)
{
	int total_length = 0;
	int j;
	for (j = 0; j < CONFIG.num_row; ++j)
		total_length += CONFIG.row[j].size + 1;
	*buf_length = total_length;

	char *buf = malloc(total_length);
	char *p = buf;
	for (j = 0; j < CONFIG.num_row; ++j){
		memcpy(p, CONFIG.row[j].chars, CONFIG.row[j].size);
		p += CONFIG.row[j].size;
		*p = '\n';
		p++;
	}
	return buf;
}

void editor_open(char *file_name)
{
	free(CONFIG.file_name);
	CONFIG.file_name = strdup(file_name);/*делает копию этой строки*/

	FILE *fp = fopen(file_name, "r");
	if(!fp) die("fopen");

	char *line = NULL;
	size_t linecap = 0;
	ssize_t line_length;
	while ((line_length = getline(&line, &linecap, fp)) != -1){
		while (line_length > 0 && (line[line_length - 1] == '\n' ||
									line[line_length - 1] == '\r'))
			line_length--;
		insert_row(CONFIG.num_row, line, line_length);
	}
	free(line);
	free(fp);
	CONFIG.dirty = 0;
}

void editor_save()
{
	if(CONFIG.file_name == NULL){
		CONFIG.file_name = input_prompt("Save as: %s (ESC to cancel)");
		if(CONFIG.file_name == NULL){
			set_status_message("Save aborted");
			return;
		}
	}

	int len;
	char *buf = editor_rows_to_string(&len);

	int fd = open(CONFIG.file_name, O_RDWR | O_CREAT, 0644);
	if(fd != -1){
		if (ftruncate(fd, len) != -1){
			if (write(fd, buf, len) == len){
				close(fd);
				free(buf);
				CONFIG.dirty = 0;
				set_status_message("%d bytes written to disk", len);
				return;
			}
		}
		close(fd);
	}

	free(buf);
	set_status_message("Can't save! I/O error: %s", strerror(errno));
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

void check_scroll()
{
	CONFIG.rx = 0;
	if(CONFIG.cy < CONFIG.num_row){
		CONFIG.rx = row_cx_to_rx(&CONFIG.row[CONFIG.cy], CONFIG.cx);
	}

	if(CONFIG.cy < CONFIG.scroll_ver){  /*Делаем так чтобы scroll_ver попал в наш экран*/
		CONFIG.scroll_ver = CONFIG.cy;
	}
	if(CONFIG.cy >= CONFIG.scroll_ver + CONFIG.screenrows){
		CONFIG.scroll_ver = CONFIG.cy - CONFIG.screenrows + 1;
	}
	if(CONFIG.rx < CONFIG.scroll_hor){
		CONFIG.scroll_hor = CONFIG.rx;
	}
	if(CONFIG.rx >= CONFIG.scroll_hor + CONFIG.screencols){
		CONFIG.scroll_hor = CONFIG.rx - CONFIG.screencols + 1;
	}
}


void draw_rows(struct append_buffer *ab)
{
	int i;
	for (i = 0; i < CONFIG.screenrows; ++i)
	{	
		int file_scroll_ver = i + CONFIG.scroll_ver;
		if(file_scroll_ver >= CONFIG.num_row){
			if (CONFIG.num_row == 0 && i == CONFIG.screenrows / 3) {
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
			int len = CONFIG.row[file_scroll_ver].rsize - CONFIG.scroll_hor;
			if(len < 0) len =0;
			if(len > CONFIG.screencols) len = CONFIG.screencols; /*Вот тут я обрезал строку(добавить потом режим переноса?)*/
			ab_append(ab, &CONFIG.row[file_scroll_ver].render[CONFIG.scroll_hor], len);
		}
		
		ab_append(ab, "\x1b[K", 3);
		
		ab_append(ab, "\r\n", 2);		
	}
}

void draw_status_bar(struct append_buffer *ab)
{
	ab_append(ab, "\x1b[7m", 4);
	char status[80], rstatus[80];
	int len = snprintf(status, sizeof(status), "%.20s - %d lines", 
		CONFIG.file_name ? CONFIG.file_name : "[No Name]", CONFIG.num_row,
		CONFIG.dirty ? "(modified)" : "");
	int rlen = snprintf(rstatus, sizeof(rstatus), "%d/%d", CONFIG.cy + 1, CONFIG.num_row);
	if(len > CONFIG.screencols) len = CONFIG.screencols;
	ab_append(ab, status, len);
	while (len < CONFIG.screencols)
	{
		if(CONFIG.screencols - len == rlen){
			ab_append(ab, rstatus, rlen);
			break;
		} else{
		ab_append(ab, " ", 1);
		len++;
		}
	}
	ab_append(ab, "\x1b[m", 3);
	ab_append(ab, "\r\n", 2);
}

void draw_status_message_bar(struct append_buffer *ab)
{
	ab_append(ab, "\x1b[K", 3);
	int msg_len = strlen(CONFIG.status_message);
	if(msg_len > CONFIG.screencols) msg_len = CONFIG.screencols;
	if(msg_len && time(NULL) - CONFIG.status_time < 5)
		ab_append(ab, CONFIG.status_message, msg_len);
}

void refresh_screen()
{
	check_scroll();

	struct append_buffer ab = ABUF_INIT;
	ab_append(&ab, "\x1b[?25l", 6);
	ab_append(&ab, "\x1b[H", 3);

	draw_rows(&ab);
	draw_status_bar(&ab);
	draw_status_message_bar(&ab);

	char buf[32];
	snprintf(buf, sizeof(buf), "\x1b[%d;%dH", CONFIG.cy - CONFIG.scroll_ver + 1, 
											  CONFIG.rx - CONFIG.scroll_hor + 1);/*Точное положение курсора*/
	ab_append(&ab, buf, strlen(buf));

	ab_append(&ab, "\x1b[?25h", 6);

	write(STDOUT_FILENO, ab.b, ab.len);
	ab_free(&ab);
}

void set_status_message(const char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(CONFIG.status_message, sizeof(CONFIG.status_message), fmt, ap);
	va_end(ap);
	CONFIG.status_time = time(NULL);
}

	/*** INPUT ***/

char *input_prompt(char *prompt)
{
	size_t buf_size = 128; /*таким образом принимаем только char*/
	char *buf = malloc(buf_size);

	size_t buf_length = 0;
	buf[0] = '\0';

	while(1){
		set_status_message(prompt, buf);
		refresh_screen(); /*ждём ввода строки*/

		int c = read_key();
		if(c == DELETE_KEY || c == CTRL_KEY('h') || c == BACKSPACE){
			if(buf_length != 0) buf[--buf_length] = '\0';
		} else if (c == '\x1b') {
			set_status_message("");
			free(buf);
			return NULL;
		} else if(c == '\r'){
			if(buf_length != 0){
				set_status_message("");
				return buf;
			}
		} else if(!iscntrl(c) && c < 128){
			if(buf_length == buf_size - 1){
				buf_size *= 2;
				buf = realloc(buf, buf_size);
			}
			buf[buf_length++] = c;
			buf[buf_length] = '\0';
		}
	}
}

void move_cursor(int key)
{
	erow *row = (CONFIG.cy >= CONFIG.num_row) ? NULL : &CONFIG.row[CONFIG.cy];

	switch(key){

		case ARROW_LEFT:
		if(CONFIG.cx != 0) /*Прверка на границы экрана*/
    		CONFIG.cx--;
    		else if (CONFIG.cy > 0){
    			CONFIG.cy--;
    			CONFIG.cx = CONFIG.row[CONFIG.cy].size; /*если налево закончилось идём в конец предыдущей*/
    		}
       	break;

    	case ARROW_RIGHT:
    	if (row && CONFIG.cx < row->size){
    		CONFIG.cx++;
    	} else if(row && CONFIG.cx == row->size){ /*как налево только направо и вниз*/
    		CONFIG.cy++;
    		CONFIG.cx = 0;
    	}
        break;
 
        case ARROW_UP:
        if(CONFIG.cy != 0)
        	CONFIG.cy--;
        break;

        case ARROW_DOWN:
        if(CONFIG.cy < CONFIG.num_row)
        	CONFIG.cy++;
        break;
	}

	row = (CONFIG.cy >= CONFIG.num_row) ? NULL : &CONFIG.row[CONFIG.cy];
	int row_len = row ? row->size : 0;
	if(CONFIG.cx > row_len){
		CONFIG.cx = row_len;
	}
}

void keypress()
{
	static int quit_times = QUIT_TIMES;

	int c = read_key();

	switch(c){
		case '\r':
		editor_insert_new_line();
		break;

		case CTRL_KEY('q'):
		if (CONFIG.dirty && quit_times > 0){
			set_status_message("WARNING!!! File has unsaved changes. "
          	"Press Ctrl-Q %d more times to quit.", quit_times);
			quit_times--;
			return;
		}
		write(STDOUT_FILENO, "\x1b[2J", 4);
		write(STDOUT_FILENO, "\x1b[H", 3);
		exit(0);
		break;

		case CTRL_KEY('s'):
		editor_save();
		break;

		case HOME_KEY:
		CONFIG.cx = 0;
		break;

		case END_KEY:
		if(CONFIG.cy < CONFIG.num_row)
			CONFIG.cx = CONFIG.row[CONFIG.cy].size;
		break;

		case BACKSPACE:
		case CTRL_KEY('h'):
		case DELETE_KEY:
		if(c == DELETE_KEY) move_cursor(ARROW_RIGHT);
		editor_delete_char();
		break;

		case PAGE_UP:
		case PAGE_DOWN:
		{
			if(c == PAGE_UP){
				CONFIG.cy = CONFIG.scroll_ver;
			} else if(c == PAGE_DOWN){
				CONFIG.cy = CONFIG.scroll_ver + CONFIG.screenrows - 1;
				if(CONFIG.cy > CONFIG.num_row) CONFIG.cy = CONFIG.num_row;
			}
		}
		break;

		case ARROW_UP:
		case ARROW_DOWN:
		case ARROW_LEFT:
		case ARROW_RIGHT:
			move_cursor(c);
			break;

		case CTRL_KEY('l'):
		case '\x1b':	
		break;

		default:
		editor_insert_char(c);
		break;	
	}

	quit_times = QUIT_TIMES;
}

	/*** INIT ***/

void init()
{
	CONFIG.cx = 0;
	CONFIG.cy = 0;
	CONFIG.rx = 0;
	CONFIG.scroll_ver = 0;
	CONFIG.scroll_hor = 0;
	CONFIG.num_row = 0;
	CONFIG.row = NULL;
	CONFIG.dirty = 0;
	CONFIG.file_name = NULL;
	CONFIG.status_message[0] = '\0';
	CONFIG.status_time = 0;

	if(get_window_size(&CONFIG.screenrows, &CONFIG.screencols) == -1)
		die("get_window_size");
	CONFIG.screenrows -= 2;
}

int main(int argc, char *argv[]) 
{
	raw_mode_on();
	init();
	if(argc >= 2){
		editor_open(argv[1]);
	}

	set_status_message("HELP: Ctrl-S = save | Ctrl-Q = quit");

	while (1){
		refresh_screen();
		keypress();
	}
  	return 0;
}