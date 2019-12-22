#include "Redactor.h"


string_of_file *fopenfile (string_of_file *arrofstr)
{
	int i = 0,j = 0;
	FILE *readfile = NULL;
	char *rptr = NULL;


	arg_separation();

	if ((comand[0] != '"') || (comand[0] == '\0'))
	{
		printf("Неверный аргумент.\n");
		printf("Введите команду заново.\n");
		printf("Аргументом должно быть имя файла заключенное в кавычки.\n");
		return NULL;
	}

	i = 1;
	j = 0;

	while (comand[i] != '\0')
	{
		comand[j] = comand[i];
		i++;
		j++;
	}
	comand[j] = '\0';

	i = 0;
	j = 0;

	if (comand[0] == '\0')
	{
		printf("Неверный аргумент.\n");
		printf("Введите команду заново.\n");
		printf("Аргументом должно быть имя файла заключенное в кавычки.\n");
		return NULL;
	}

	while (comand[i] != '\0')
	{
		if ((comand[i] == '"') && (comand[i + 1] != '\0'))
		{
			printf("Неверный аргумент.\n");
			printf("Введите команду заново.\n");
			printf("Аргументом должно быть имя файла заключенное в кавычки.\n");
			return NULL;
		}
		i++;
	}

	if (i != 0)
		if (comand[i - 1] != '"')
		{
			printf("Неверный аргумент.\n");
			printf("Введите команду заново.\n");
			printf("Аргументом должно быть имя файла заключенное в кавычки.\n");
			return NULL;
		}

	i = 0;

	while (comand[i] != '\0')
	{
		if (comand[i] == '"')
		{
			comand[i] = '\0';
			break;
		}
		i++;
	}

	readfile = fopen(comand,"r");
	if (readfile == NULL)
	{
		printf("Такого файла не существует.\n");
		return NULL;
	}
	else
	{
		for (i = 0;i < countofstr; i++)
		{
			free(arrofstr[i].str);
		}
		free(arrofstr);
		arrofstr = NULL;

		fclose(infile);

		infile = readfile;
		readfile = NULL;

		countofstr = 0;

		i = 0;
		free(filename);
		filename = NULL;

		while (comand[i] != '\0')
			{
				rptr = (char *)realloc(filename,(i+1)*sizeof(char));
				if (rptr == NULL)
				{
					free(filename);
					filename = NULL;
					printf("Недостаточно памяти.\n");
					memory_err_flag = 1;
					return NULL;
				}
				else
				{
					filename = rptr;
					rptr = NULL;
				}

				filename[i] = comand[i];  

				i++;
			}

		rptr=(char *)realloc(filename,(i+1)*sizeof(char));
		if (rptr == NULL)
		{
			free(filename);
			filename = NULL;
			printf("Недостаточно памяти.\n");
			memory_err_flag = 1;
			return NULL;
		}
		else
		{
			filename =rptr;
			rptr = NULL; 
		}

		filename[i] = '\0';
		set_name_flag = 1;

		save_file_flag = 0;
		emptyfile = 1;

		arrofstr = file_toram(infile);

		printf("File with name '%s' was opened.\n",filename);

		return arrofstr;
	}
}


string_of_file *freadfile (string_of_file *arrofstr)
{
	int i = 0,j = 0;
	FILE *readfile = NULL;


	arg_separation();

	if ((comand[0] != '"') || (comand[0] == '\0'))
	{
		printf("Неверный аргумент.\n");
		printf("Введите команду заново.\n");
		printf("Аргументом должно быть имя файла заключенное в кавычки.\n");
		return NULL;
	}

	i = 1;
	j = 0;

	while (comand[i] != '\0')
	{
		comand[j] = comand[i];
		i++;
		j++;
	}
	comand[j] = '\0';

	i = 0;
	j = 0;

	if (comand[0] == '\0')
	{
		printf("Неверный аргумент.\n");
		printf("Введите команду заново.\n");
		printf("Аргументом должно быть имя файла заключенное в кавычки.\n");
		return NULL;
	}

	while (comand[i] != '\0')
	{
		if ((comand[i] == '"') && (comand[i+1] != '\0'))
		{
			printf("Неверный аргумент.\n");
			printf("Введите команду заново.\n");
			printf("Аргументом должно быть имя файла заключенное в кавычки.\n");
			return NULL;
		}
		i++;
	}

	if (i != 0)
		if (comand[i-1] != '"')
		{
			printf("Неверный аргумент.\n");
			printf("Введите команду заново.\n");
			printf("Аргументом должно быть имя файла заключенное в кавычки.\n");
			return NULL;
		}

	i = 0;

	while (comand[i] != '\0')
	{
		if (comand[i] == '"')
		{
			comand[i] = '\0';
			break;
		}
		i++;
	}

	i = 0;

	readfile = fopen(comand,"r");
	if (readfile == NULL)
	{
		printf("Такого файла не существует.\n");
		return NULL;
	}
	else
	{
		for (i = 0;i < countofstr; i++)
		{
			free(arrofstr[i].str);
			arrofstr[i].str = NULL;
		}
		free(arrofstr);
		arrofstr = NULL;

		fclose(infile);

		infile = readfile;
		readfile = NULL;

		countofstr = 0;

		save_file_flag = 0;
		emptyfile = 1;

		arrofstr = file_toram(infile);

		printf("File was opened, name didn't memorized.\n");

		return arrofstr;
	}
}

void ffilesave (string_of_file *arrofstr)
{
	int i = 0, j = 0;
	FILE *outfile = NULL;


	arg_separation();

	if ((comand[0] != '"') && (comand[0] != '\0'))
	{
		printf("Неверный аргумент.\n");
		printf("Введите команду заново.\n");
		printf("Аргументом должно быть имя файла заключенное в кавычки\nили аргумент должен отсутствовать.\n");
		return;
	}

	if ((comand[0] == '"') && (comand[1] == '\0'))
	{
		printf("Неверный аргумент.\n");
		printf("Введите команду заново.\n");
		printf("Аргументом должно быть имя файла заключенное в кавычки.\n");
		return;
	}

	if (comand[0] == '"')	
	{
		i = 1;
		j = 0;

		while (comand[i] != '\0')
		{
			comand[j] = comand[i];
			i++;
			j++;
		}
		comand[j] = '\0';
	}

	i = 0;
	j = 0;

	while (comand[i] != '\0')
	{
		if ((comand[i] == '"') && (comand[i+1] != '\0'))
		{
			printf("Неверный аргумент.\n");
			printf("Введите команду заново.\n");
			printf("Аргументом должно быть имя файла заключенное в кавычки\nили аргумент должен отсутствовать.\n");
			return;
		}
		i++;
	}

	
	if (i != 0)
		if (comand[i-1] != '"')
		{
			printf("Неверный аргумент.\n");
			printf("Введите команду заново.\n");
			printf("Аргументом должно быть имя файла заключенное в кавычки\nили аргумент должен отсутствовать.\n");
			return;
		}

	i = 0;

	while (comand[i] != '\0')
	{
		if (comand[i] == '"')
		{
			comand[i] = '\0';
			break;
		}
		i++;
	}

	if ((comand[0] == '\0') && (set_name_flag == 0))
	{
		printf("Не ассоциированно имя файла.\n");
		printf("Воспользуйтесь функцией 'set name'.\n");
		return;
	}

	i = 0;

	if (set_name_flag == 1)
	{
		fclose(infile);
		infile = NULL;
		infile = fopen(filename,"w");

		for (i = 0;i < countofstr; i++)
			for (j = 0; j<arrofstr[i].stringlenght; j++)
				fputc((int)arrofstr[i].str[j],infile);

		save_file_flag = 1;

		printf("File with name '%s' was saved.\n", filename);

		return;
	}
	else
	if ((set_name_flag == 0) && (comand[0] != '\0'))
	{
		fclose(infile);
		infile = NULL;
		infile = fopen(comand,"w+");

		for (i = 0; i < countofstr; i++)
			for (j = 0; j < arrofstr[i].stringlenght; j++)
				fputc((int)arrofstr[i].str[j],infile);

		save_file_flag=1;

		printf("File with name '%s' was saved.\n",filename);

		return;
	}
}

void fset_name()
{
	int i = 0, j = 0;
	char *rptr = NULL;


	arg_separation();

	if (comand[0] == '\0')
	{
		printf("Ассоциация с файлом удалена.\n");
		free(filename);
		filename = NULL;
		set_name_flag = 0;
		return;
	}

	if (comand[0] != '"')
	{
		printf("Неверный аргумент.\n");
		printf("Введите команду заново.\n");
		printf("Аргументом должно быть имя файла заключенное в кавычки.\n");
		return;
	}

	i = 1;
	j = 0;

	while (comand[i] != '\0')
	{
		comand[j] = comand[i];
		i++;
		j++;
	}
	comand[j] = '\0';

	i = 0;
	j = 0;

	while (comand[i] != '\0')
	{
		if ((comand[i] == '"') && (comand[i+1] != '\0'))
		{
			printf("Неверный аргумент.\n");
			printf("Введите команду заново.\n");
			printf("Аргументом должно быть имя файла заключенное в кавычки.\n");
			return;
		}
		i++;
	}

	if (comand[i-1] != '"')
	{
		printf("Неверный аргумент.\n");
		printf("Введите команду заново.\n");
		printf("Аргументом должно быть имя файла заключенное в кавычки.\n");
		return;
	}

	i = 0;

	while (comand[i] != '\0')
	{
		if (comand[i] == '"')
		{
			comand[i] = '\0';
			break;
		}
		i++;
	}

	i = 0;
	free(filename);

	while (comand[i] != '\0')
	{
		rptr = (char *)realloc(filename,(i+1)*sizeof(char));
		if (rptr == NULL)
		{
			free(filename);
			filename = NULL;
			printf("Недостаточно памяти.\n");
			memory_err_flag = 1;
			return;
		}
		else
		{
			filename = rptr;
			rptr = NULL;
		}

		filename[i] = comand[i];  

		i++;
	}

	rptr = (char *)realloc(filename,(i+1)*sizeof(char));
	if (rptr == NULL)
	{
		free(filename);
		filename = NULL;
		printf("Недостаточно памяти.\n");
		memory_err_flag = 1;
		return;
	}
	else
	{
		filename = rptr;
		rptr = NULL;
	}

	filename[i] = '\0';
	set_name_flag = 1;

	printf("С файлом ассоциированно имя '%s'.\n",filename);
	return;
}

void fhelp ()
{
	int savecountofstr = 0, i = 0;
	string_of_file *helpfileptr = NULL;
	FILE *helpfile = NULL;

	
	arg_separation();

	if (comand[0] != '\0')
	{
		printf("Такой команды не существует.\n");
		printf("Введите команду заново.\n");
		return;
	}
	else
	{
		helpfile = fopen("kul_help", "r");

		savecountofstr = countofstr;
		countofstr = 0;
		helpfileptr = file_toram(helpfile);

		fprint_pages(helpfileptr);

		for (i = 0; i < countofstr; i++)
		{
			free(helpfileptr[i].str);
			helpfileptr[i].str = NULL;
		}
		free(helpfileptr);
		helpfileptr = NULL;

		fclose(helpfile);
		helpfile = NULL;

		countofstr = savecountofstr;
	}
	return;
}
