#include "Redactor.h"

int termstr = 0, countofstr = 0, termwid = 0, memory_err_flag = 0, set_name_flag = 0, comandrest = 0,
save_file_flag = 0, end_flag = 0, charnum = 0, setwrapflag = 1,
emptyfile = 1, exitflag = 0, comand_read_flag = 1, settabwidthflag = 8;

FILE *infile = NULL;
char comand_buf[COMANDLEN+1];
char *comand = NULL,*filename = NULL;


int main (int argc, char const *argv[])
{
	string_of_file *arrofstr = NULL;
	int i = 0;
	char new_file[] = "new_file";
	char *rptr = NULL;
	struct winsize term;

	for (i = 0;i < COMANDLEN + 2; i++)
		comand_buf[i]=0;

	ioctl(0,TIOCGWINSZ,&term);
	termstr = term.ws_row;
	termwid = term.ws_col;

	
	printf("KULEDITOR welcomes you!\n");

	if (argc != 1)
		infile = fopen(argv[1], "r");

	if (infile == NULL)
	{
		printf("Такого файла не существует!\n");
		printf("Создан пустой файл с именем 'new_file'.\n");
		printf("Вы можете ввести файл ещё раз\nс помощью функций 'read'/'open'.\n");

		infile = fopen("new_file", "w+");
		filename = malloc(9*sizeof(char));
		if (filename == NULL)
		{
			printf("Недостаточно памяти.\n");
			memory_err_flag = 1;
			return -1;
		}

		for (i = 0; i < 9; i++)
			filename[i] = new_file[i];

		set_name_flag = 1;

		save_file_flag=0;
	}
	else
	{
		i = 0;

		while (argv[1][i]!='\0')
		{
			rptr = (char *)realloc(filename,(i+1) * sizeof(char));
			if (rptr == NULL)
			{
				free(filename);
				filename = NULL;
				printf("Недостаточно памяти.\n");
				memory_err_flag=1;
				return -1;
			}
			else
			{
				filename = rptr;
				rptr = NULL;
			}

			filename[i] = argv[1][i];  

			i++;
		}

		rptr = (char *)realloc(filename, (i+1)*sizeof(char));
		if (rptr == NULL)
		{
			free(filename);
			filename = NULL;
			printf("Недостаточно памяти.\n");
			memory_err_flag=1;
			return -1;
		}
		else
		{
			filename = rptr;
			rptr = NULL;
		}

		filename[i] = '\0';
		set_name_flag = 1;

		save_file_flag = 0;

		arrofstr = file_toram(infile);
	}

	comand_analize(arrofstr);

	return 0;
}