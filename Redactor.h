#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>

#define STRLEN 256
#define COMANDLEN 21

/*СТРУКТУРА СТРОКИ*/
typedef struct  
{
	char *str;
	int stringlenght;
}string_of_file;

extern int termstr, countofstr, termwid , memory_err_flag , set_name_flag , comandrest ,
save_file_flag , end_flag , charnum , setwrapflag ,
emptyfile , exitflag , comand_read_flag , settabwidthflag ;

extern FILE *infile;
extern char comand_buf[COMANDLEN+1];
extern char *comand, *filename;

int cmpstr (char *str1, char *str2);
string_of_file *file_toram (FILE *f);
void comand_analize (string_of_file *arrofstr);
void fprint_pages (string_of_file *arrofstr);
void fprint_range (string_of_file *arrofstr);
void arg_separation();
string_of_file *freadfile (string_of_file *arrofstr);
string_of_file *fopenfile (string_of_file *arrofstr);
void ffilesave (string_of_file *arrofstr);
void fset_name();
int fset_tabwidth ();
int fset_wrap ();
void fhelp ();
string_of_file *finsert_symbol (string_of_file *arrofstr);
string_of_file *finsert_after (string_of_file *arrofstr);
string_of_file *fedit_string (string_of_file *arrofstr);
string_of_file *fdelete_range (string_of_file *arrofstr);
string_of_file *freplace_substring (string_of_file *arrofstr);

