#include "Redactor.h"

string_of_file *fedit_string (string_of_file *arrofstr)
{
	int i = 0, j = 0, arg1 = 0, arg2 = 0, arg3 = 0, len = 0, spacecount = 0, symbolflag = 0;
	char *ptr = NULL;
	string_of_file str2;
	string_of_file *arrofstr2 = NULL;
	char *rptr = NULL;
	string_of_file *rarrofstr = NULL;


	while (comand[i] != '\0')
	{
		if ((comand[i] == '\\') && (comand[i + 1] == '#'))
			symbolflag = 1;

		i++;
	}

	i = 0;

	arg_separation();
	
	if (((comand[0] > '9') || (comand[0]<'1'))&&(comand[0]!='\0'))
	{
		printf("Неверные аргументы.\n");
		printf("Введите команду заново.\n");
		printf("Аргументами должны быть два числа больше 0 и символ.\n");
		return arrofstr;
	}

	while (comand[i] != '\0')
	{
		if (comand[i] == ' ')
			spacecount++;

		i++;
	}

	if (spacecount != 2)
	{
		printf("Неверные аргументы.\n");
		printf("Введите команду заново.\n");
		printf("Аргументами должны быть два числа больше 0 и символ.\n");
		return arrofstr;
	}

	i = 0;
	spacecount = 0;

	while (comand[i] != '\0')
	{
		if ((((comand[i] > '9') || (comand[i] < '0')) && (comand[i] != ' ') && (comand[i] != '\\')) && (comand[i + 1] != '\0'))
		{
			printf("Неверные аргументы.\n");
			printf("Введите команду заново.\n");
			printf("Аргументами должны быть два числа больше 0 и символ.\n");
			return arrofstr;
		}
		i++;
	} 

	i = 0;

	while (comand[i] != ' ')
	{
		if ((comand[i] > '9') || (comand[i] < '0') || (comand[0] == '0'))
		{
			printf("Неверные аргументы.\n");
			printf("Введите команду заново.\n");
			printf("Аргументами должны быть два числа больше 0 и символ.\n");
			return arrofstr;
		}
		i++;
	}

	arg1 = atoi(comand);
	comandrest = i;
	arg_separation();

	i = 0;

	while (comand[i] != ' ')
	{
		if ((comand[i] > '9') || (comand[i] < '0') || (comand[0] == '0'))
		{
			printf("Неверные аргументы.\n");
			printf("Введите команду заново.\n");
			printf("Аргументами должны быть два числа больше 0 и символ.\n");
			return arrofstr;
		}
		i++;
	}

	arg2 = atoi(comand);
	comandrest = i;
	arg_separation();

	i = 0;

	if (symbolflag == 1)
	{
		comand[1] = '#';
		comand[2] = '\0';
	}

	if ((comand[0] == '\\') && (comand[1] != '\0') && (comand[1] != '#') && (comand[1] != 't') && (comand[1] != 'n'))
	{
		printf("Неверные аргументы.\n");
		printf("Введите команду заново.\n");
		printf("Аргументами должны быть два числа больше 0 и символ.\n");
		return arrofstr;
	}
	else
	if (comand[1] != '\0')
	{
		if (comand[1] == '#')
			arg3 = '#';
		else
		if (comand[1] == 't')
			arg3 = '\t';
		else
		if (comand[1] == 'n')
			arg3 = '\n';
	}
	else
	if (comand[1] == '\0')
	{
		arg3 = comand[0];
	}

	if (arg1 > countofstr)
	{
		printf("Строки с таким номером в файле не существует.\n");
		printf("Введите команду заново.\n");
		return arrofstr;
	}


	len = arrofstr[arg1 - 1].stringlenght;
	ptr = arrofstr[arg1 - 1].str;

	if (arg2 > (len - 1))
	{
		printf("В строке нет символа с таким номером.\n");
		printf("Введите команду заново.\n");
		ptr = NULL;
		return arrofstr;
	}

	ptr[arg2 - 1] = (char) arg3;

	if (arg3 == '\n')
	{
		i = 0;

		while (ptr[i] != '\n')
		{
			i++;
		} 

		arrofstr[arg1 - 1].stringlenght = i + 1;

		str2.str = NULL;
		str2.stringlenght = 0;
		i++;
		while (ptr[i] != '\n')
		{
			str2.stringlenght++;
			rptr = (char *)realloc(str2.str,str2.stringlenght*sizeof(char));
			if (rptr == NULL)
			{
				free(str2.str);
				str2.str = NULL;
				printf("Недостаточно памяти.\n");
				memory_err_flag = 1;
				return NULL;
			}
			else
			{
				str2.str = rptr;
				rptr = NULL;
			}

			str2.str[str2.stringlenght - 1] = ptr[i];

			i++;
		}
		str2.stringlenght++;
		rptr = (char *)realloc(str2.str,str2.stringlenght*sizeof(char));
		if (rptr == NULL)
		{
			free(str2.str);
			str2.str = NULL;
			printf("Недостаточно памяти.\n");
			memory_err_flag = 1;
			return NULL;
		}
		else
		{
			str2.str=rptr;
			rptr=NULL;
		}
		str2.str[str2.stringlenght-1]='\n';

		countofstr++;

		arrofstr2=(string_of_file *)realloc(arrofstr,countofstr*sizeof(string_of_file));
		if (arrofstr2==NULL)
		{
			printf("Недостаточно памяти.\n");
			memory_err_flag = 1;
			return arrofstr;
		}
		else
		{
			arrofstr = arrofstr2;
			arrofstr2 = NULL;
		}

		i = countofstr;
		while (i != (arg1 + 1))
		{
			arrofstr[i - 1].str = arrofstr[i - 2].str;
			arrofstr[i - 1].stringlenght=arrofstr[i - 2].stringlenght;

			i--;
		}

		arrofstr[arg1].str = str2.str;
		arrofstr[arg1].stringlenght = str2.stringlenght;
	}

	ptr = NULL;

	printf("Symbol was successfully replaced in %i string on %i position.\n",arg1,arg2);

	return arrofstr;
}

string_of_file *finsert_symbol (string_of_file *arrofstr)
{
	int i = 0, j = 0, arg1 = 0, arg2 = 0, arg3 = 0, len = 0, spacecount = 0, symbolflag = 0;
	char *ptr = NULL;
	string_of_file str2;
	string_of_file *arrofstr2 = NULL;
	char *rptr = NULL;
	string_of_file *rarrofstr = NULL;


	while (comand[i] != '\0')
	{
		if ((comand[i] == '\\') && (comand[i + 1] == '#'))
			symbolflag = 1;

		i++;
	}

	i = 0;

	arg_separation();
	
	if (((comand[0] > '9') || (comand[0] < '1')) && (comand[0] != '\0'))
	{
		printf("Неверные аргументы.\n");
		printf("Введите команду заново.\n");
		printf("Аргументами должны быть два числа больше 0 и символ.\n");
		printf("Введите '0' на месте второго аргумента,\nчтобы добавить символ в начало строки.\n");
		return arrofstr;
	}

	while (comand[i] != '\0')
	{
		if (comand[i] == ' ')
			spacecount++;

		i++;
	}

	if (spacecount != 2)
	{
		printf("Неверные аргументы.\n");
		printf("Введите команду заново.\n");
		printf("Аргументами должны быть два числа больше 0 и символ.\n");
		printf("Введите '0' на месте второго аргумента,\nчтобы добавить символ в начало строки.\n");
		return arrofstr;
	}

	i = 0;
	spacecount = 0;

	while (comand[i] != '\0')
	{
		if ((((comand[i] > '9') || (comand[i]<'0')) && (comand[i] != ' ') && (comand[i] != '\\')) && (comand[i + 1] != '\0'))
		{
			printf("Неверные аргументы.\n");
			printf("Введите команду заново.\n");
			printf("Аргументами должны быть два числа больше 0 и символ.\n");
			printf("Введите '0' на месте второго аргумента,\nчтобы добавить символ в начало строки.\n");
			return arrofstr;
		}
		i++;
	} 

	i = 0;

	while (comand[i] != ' ')
	{
		if ((comand[i] > '9') || (comand[i] < '0') || (comand[0] == '0'))
		{
			printf("Неверные аргументы.\n");
			printf("Введите команду заново.\n");
			printf("Аргументами должны быть два числа больше 0 и символ.\n");
			printf("Введите '0' на месте второго аргумента,\nчтобы добавить символ в начало строки.\n");
			return arrofstr;
		}
		i++;
	}

	arg1 = atoi(comand);
	comandrest = i;
	arg_separation();

	i = 0;

	while (comand[i] != ' ')
	{
		if ((comand[i] > '9') || (comand[i] < '0'))
		{
			printf("Неверные аргументы.\n");
			printf("Введите команду заново.\n");
			printf("Аргументами должны быть два числа больше 0 и символ.\n");
			printf("Введите '0' на месте второго аргумента,\nчтобы добавить символ в начало строки.\n");
			return arrofstr;
		}
		i++;
	}

	arg2 = atoi(comand);
	comandrest = i;
	arg_separation();

	i = 0;

	if (symbolflag == 1)
	{
		comand[1] = '#';
		comand[2] = '\0';
	}

	if ((comand[0] == '\\') && (comand[1] != '\0') && (comand[1] != '#') && (comand[1] != 't') && (comand[1] != 'n'))
	{
		printf("Неверные аргументы.\n");
		printf("Введите команду заново.\n");
		printf("Аргументами должны быть два числа больше 0 и символ.\n");
		printf("Введите '0' на месте второго аргумента,\nчтобы добавить символ в начало строки.\n");
		return arrofstr;
	}
	else
	if (comand[1] != '\0')
	{
		if (comand[1] == '#')
			arg3 = '#';
		else
		if (comand[1] == 't')
			arg3 = '\t';
		else
		if (comand[1] == 'n')
			arg3 = '\n';
	}
	else
	if (comand[1] == '\0')
	{
		arg3 = comand[0];
	}

	if (arg1 > countofstr)
	{
		printf("Строки с таким номером в файле не существует.\n");
		printf("Введите команду заново.\n");
		return arrofstr;
	}


	len = arrofstr[arg1-1].stringlenght;
	ptr = arrofstr[arg1-1].str;

	if (arg3 != '\n')
	{
		if (arg2 < len)
		{	
			len++;
			i = len;

			rptr = (char *)realloc(ptr,len*sizeof(char));
			if (rptr == NULL)
			{
				free(ptr);
				ptr = NULL;
				printf("Недостаточно памяти.\n");
				memory_err_flag = 1;
				return NULL;
			}
			else
			{
				ptr = rptr;
				rptr = NULL;
			}

			while (i != (arg2 + 1))
			{
				ptr[i - 1] = ptr[i - 2];

				i--;
			}

			ptr[arg2] = (char)arg3;

			arrofstr[arg1 - 1].stringlenght = len;
			arrofstr[arg1 - 1].str = ptr;
		}
		else
		{
			len++;
			i = len;

			rptr = (char *)realloc(ptr,len*sizeof(char));
			if (rptr == NULL)
			{
				free(ptr);
				ptr = NULL;
				printf("Недостаточно памяти.\n");
				memory_err_flag = 1;
				return NULL;
			}
			else
			{
				ptr = rptr;
				rptr = NULL;
			}

			ptr[i - 1] = ptr[i - 2];

			ptr[i - 2] = (char)arg3;

			arrofstr[arg1 - 1].stringlenght = len;
			arrofstr[arg1 - 1].str = ptr;
		}
	}
	else
	{
		if (arg2 < len)
		{	
			len++;
			i = len;

			rptr = (char *)realloc(ptr, len*sizeof(char));
			if (rptr == NULL)
			{
				free(ptr);
				ptr = NULL;
				printf("Недостаточно памяти.\n");
				memory_err_flag = 1;
				return NULL;
			}
			else
			{
				ptr = rptr;
				rptr = NULL;
			}

			while (i != (arg2 + 1))
			{
				ptr[i - 1] = ptr[i - 2];

				i--;
			}

			ptr[arg2] = (char) arg3;

			arrofstr[arg1 - 1].str = ptr;
		}
		else
		{
			len++;
			i = len;

			rptr = (char *)realloc(ptr,len*sizeof(char));
			if (rptr == NULL)
			{
				free(ptr);
				ptr = NULL;
				printf("Недостаточно памяти.\n");
				memory_err_flag = 1;
				return NULL;
			}
			else
			{
				ptr = rptr;
				rptr = NULL;
			}

			ptr[i - 1] = ptr[i - 2];

			ptr[i - 2] = (char) arg3;

			arrofstr[arg1 - 1].str = ptr;
		}

		i = 0;

		while (ptr[i] != '\n')
		{
			i++;
		} 

		arrofstr[arg1 - 1].stringlenght = i + 1;

		str2.str = NULL;
		str2.stringlenght = 0;
		i++;
		while (ptr[i] != '\n')
		{
			str2.stringlenght++;
			rptr = (char *)realloc(str2.str,str2.stringlenght*sizeof(char));
			if (rptr == NULL)
			{
				free(str2.str);
				str2.str = NULL;
				printf("Недостаточно памяти.\n");
				memory_err_flag = 1;
				return NULL;
			}
			else
			{
				str2.str = rptr;
				rptr = NULL;
			}

			str2.str[str2.stringlenght - 1] = ptr[i];

			i++;
		}
		str2.stringlenght++;
		rptr = (char *)realloc(str2.str,str2.stringlenght*sizeof(char));
		if (rptr == NULL)
		{
			free(str2.str);
			str2.str = NULL;
			printf("Недостаточно памяти.\n");
			memory_err_flag = 1;
			return NULL;
		}
		else
		{
			str2.str = rptr;
			rptr = NULL;
		}
		str2.str[str2.stringlenght - 1] = '\n';

		countofstr++;

		arrofstr2 = (string_of_file *)realloc(arrofstr,countofstr*sizeof(string_of_file));
		if (arrofstr2 == NULL)
		{
			printf("Недостаточно памяти.\n");
			memory_err_flag=1;
			return arrofstr;
		}
		else
		{
			arrofstr = arrofstr2;
			arrofstr2 = NULL;
		}

		i = countofstr;
		while (i != (arg1 + 1))
		{
			arrofstr[i-1].str = arrofstr[i-2].str;
			arrofstr[i-1].stringlenght = arrofstr[i-2].stringlenght;

			i--;
		}

		arrofstr[arg1].str = str2.str;
		arrofstr[arg1].stringlenght = str2.stringlenght;
	}

	ptr = NULL;

	printf("Symbol was successfully inserted in %i string on %i position.\n", arg1, arg2);

	return arrofstr;
}

string_of_file *finsert_after (string_of_file *arrofstr)
{
	int i = 0, j = 0, k = 0, arg1 = 0, letter = 0, multi_str_flag = 0, new_str_flag = 0, spaceflag = 0,firstinflag = 0;
	string_of_file insertstr;
	char *ptr = NULL;
	char *rptr = NULL;
	string_of_file *rarrofstr = NULL;

	arg_separation();

	if (((comand[0] > '9') || (comand[0] < '0')) && (comand[0] != '"'))
	{
		printf("Неверные аргументы.\n");
		printf("Введите команду заново.\n");
		printf("Аргументами должны быть число больше или равное 0\nи строка, заключенная в двойные кавычки\nили несколько строк заключенные в тройные кавычки.\n");
		return arrofstr;
	}

	i = 0;

	if ((comand[0] >= '0') && (comand[0] <= '9'))
	{
		arg1 = atoi(comand);

		while (comand[i] != ' ')
		{
			if ((comand[i] > '9') || (comand[i] < '0'))
			{
				printf("Неверные аргументы.\n");
				printf("Введите команду заново.\n");
				printf("Аргументами должны быть число больше или равное 0\nи строка, заключенная в двойные кавычки\nили несколько строк заключенные в тройные кавычки.\n");
				return arrofstr;
			}
			i++;
		}
	}
	else
	{
		arg1 = -1;
	}

	if (arg1 > countofstr)
	{
		printf("Неверные аргументы.\n");
		printf("Введите команду заново.\n");
		printf("Аргументами должны быть число больше или равное 0\nи строка, заключенная в двойные кавычки\nили несколько строк заключенные в тройные кавычки.\n");
		return arrofstr;
	}

	i = 0;

	if (arg1 != -1)
	{
		while ((comand[i] != ' '))
		{
			i++;
		}

		comandrest = i;
		arg_separation();
	}

	if (comand[0] != '"')
	{
		printf("Неверные аргументы.\n");
		printf("Введите команду заново.\n");
		printf("Аргументами должны быть число больше или равное 0\nи строка, заключенная в двойные кавычки\nили несколько строк заключенные в тройные кавычки.\n");
		return arrofstr;
	}

	if (comand[1] != '"')
	{
		printf("Неверные аргументы.\n");
		printf("Введите команду заново.\n");
		printf("Аргументами должны быть число больше или равное 0\nи строка, заключенная в двойные кавычки\nили несколько строк заключенные в тройные кавычки.\n");
		return arrofstr;
	}

	if (comand[2] == '"')
	{
		multi_str_flag = 1;
		i = 3;
	}
	else
	{
		multi_str_flag = 0;
		i = 2;
	}

	j = 0;

	while (comand[i] != '\0')
	{
		comand[j] = comand[i];
		i++;
		j++;
	}
	comand[j] = '\0';

	if ((comand[0] == '\0') && (multi_str_flag == 1))
	{
		free(comand);
		comand = NULL;

		i = -1;

		letter = (int)getchar();	
		while (letter != '\n')
		{
			if (((letter==' ')||(letter=='	')) && (spaceflag == 0))
			{
				i++;
				rptr = (char *)realloc(comand,(i+1)*sizeof(char));
				if (rptr == NULL)
				{
					printf("Недостаточно памяти.\n");
					memory_err_flag = 1;
					return NULL;
				}
				else
				{
					comand = rptr;
					rptr = NULL;
				}

				comand[i] = ' ';
				spaceflag = 1;
			}
			else
			if ((letter !=' ')&&(letter!='	'))
			{
				i++;
				rptr=(char *)realloc(comand,(i+1)*sizeof(char));
				if (rptr == NULL)
				{
					printf("Недостаточно памяти.\n");
					memory_err_flag = 1;
					return NULL;
				}
				else
				{
					comand = rptr;
					rptr = NULL;
				}

				comand[i] = letter;
				spaceflag = 0;
			}

			letter = (int)getchar();		
		}

		i++;
		rptr = (char *)realloc(comand,(i+1)*sizeof(char));
		if (rptr == NULL)
		{
			printf("Недостаточно памяти.\n");
			memory_err_flag = 1;
			return NULL;
		}
		else
		{
			comand = rptr;
			rptr = NULL;
		}

		comand[i] = '\0';
	}
	else
	if (comand[0] == '"')
	{
		printf("Неверные аргументы.\n");
		printf("Введите команду заново.\n");
		printf("Аргументами должны быть число больше или равное 0\nи строка, заключенная в двойные кавычки\nили несколько строк заключенные в тройные кавычки.\n");
		return arrofstr;
	}

	i = 0;
	j = 0;
	k = 0;

	if ((multi_str_flag == 0) && (comand[0] == '\0'))
	{
		printf("Неверные аргументы.\n");
		printf("Введите команду заново.\n");
		printf("Аргументами должны быть число больше или равное 0\nи строка, заключенная в двойные кавычки\nили несколько строк заключенные в тройные кавычки.\n");
		return arrofstr;
	}

	/*ОДНОСТРОЧНЫЙ РЕЖИМ*/
	if (multi_str_flag == 0)                         
	{
		while (comand[i] != '\0')
		{
			if ((comand[i] == '\\') && (comand[i + 1] != 'n') && (comand[i + 1] != 't') && (comand[i + 1] != '\\') 
				&& (comand[i +1 ] != '"') && (comand[i + 1] != 'r') && ((i != 0) && comand[i - 1] != '\\'))
			{	
				printf("Неверные аргументы.\n");
				printf("Введите команду заново.\n");
				printf("Аргументами должны быть число больше или равное 0\nи строка, заключенная в двойные кавычки\nили несколько строк заключенные в тройные кавычки.\n");
				return arrofstr;
			}

			i++;
		}

		if (i - 3 >= 0)
		{
			if ((comand[i - 1] != '"') || (comand[i - 2] != '"'))
			{
				printf("Неверные аргументы.\n");
				printf("Введите команду заново.\n");
				printf("Аргументами должны быть число больше или равное 0\nи строка, заключенная в двойные кавычки\nили несколько строк заключенные в тройные кавычки.\n");
				return arrofstr;
			}
		}
		else
		{
			printf("Неверные аргументы.\n");
			printf("Введите команду заново.\n");
			printf("Аргументами должны быть число больше или равное 0\nи строка, заключенная в двойные кавычки\nили несколько строк заключенные в тройные кавычки.\n");
			return arrofstr;
		}

		comand[i - 2] = '\0';

		if (comand[0] == '\0')
		{
			printf("Неверные аргументы.\n");
			printf("Введите команду заново.\n");
			printf("Аргументами должны быть число больше или равное 0\nи строка, заключенная в двойные кавычки\nили несколько строк заключенные в тройные кавычки.\n");
			return arrofstr;
		}

		if (i - 4 >= 0)
		{
			if ((comand[i - 3] == '\\') && (comand[i - 4] != '\\'))
			{
				printf("Неверные аргументы.\n");
				printf("Введите команду заново.\n");
				printf("Аргументами должны быть число больше или равное 0\nи строка, заключенная в двойные кавычки\nили несколько строк заключенные в тройные кавычки.\n");
				return arrofstr;
			}
		}

		i = 0;

		while (comand[i] != '\0')
		{
			if (comand[0] == '"')
			{
				printf("Неверные аргументы.\n");
				printf("Введите команду заново.\n");
				printf("Аргументами должны быть число больше или равное 0\nи строка, заключенная в двойные кавычки\nили несколько строк заключенные в тройные кавычки.\n");
				return arrofstr;
			}

			if ((i != 0) && (comand[i] == '"') && (comand[i -1 ] != '\\'))
			{
				printf("Неверные аргументы.\n");
				printf("Введите команду заново.\n");
				printf("Аргументами должны быть число больше или равное 0\nи строка, заключенная в двойные кавычки\nили несколько строк заключенные в тройные кавычки.\n");
				return arrofstr;
			}

			i++;
		}

		i = 0;
		k = 0;
		j = 0;

		while (1)
		{
			i = k;

			insertstr.stringlenght = 0;

			while (comand[i] != '\0')
			{
				insertstr.stringlenght++;
				rptr = (char *)realloc(ptr,insertstr.stringlenght*sizeof(char));
				if (rptr == NULL)
				{
					free(ptr);
					ptr = NULL;
					printf("Недостаточно памяти.\n");
					memory_err_flag = 1;
					return NULL;
				}
				else
				{
					ptr = rptr;
					rptr = NULL;
				}

				if (comand[i] == '\\')
				{
					if (comand[i + 1] == 't')
					{
						ptr[insertstr.stringlenght-1] = '\t';
						i++;
					}
					else
					if (comand[i + 1] == '\\')
					{
						ptr[insertstr.stringlenght-1] = '\\';
						i++;
					}
					else
					if (comand[i + 1] == 'r')
					{
						ptr[insertstr.stringlenght - 1] = '\r';
						i++;
					}
					else
					if (comand[i + 1] == '"')
					{
						ptr[insertstr.stringlenght - 1] = '"';
						i++;
					}
					else
					if (comand[i + 1] == 'n')
					{
						ptr[insertstr.stringlenght - 1] = '\n';
						i++;
						i++;
						new_str_flag  = 1;
						break;
					}
				}
				else
				{
					ptr[insertstr.stringlenght - 1] = comand[i];
				}

				i++;
			}

			k=i;

			if (new_str_flag == 0)
			{
				insertstr.stringlenght++;
				rptr = (char *)realloc(ptr,insertstr.stringlenght*sizeof(char));
				if (rptr == NULL)
				{
					free(ptr);
					ptr = NULL;
					printf("Недостаточно памяти.\n");
					memory_err_flag = 1;
					return NULL;
				}
				else
				{
					ptr = rptr;
					rptr = NULL;
				}
				ptr[insertstr.stringlenght-1] = '\n';
			}

			insertstr.str = ptr;
			ptr = NULL;

			countofstr++;

			rarrofstr = (string_of_file *)realloc(arrofstr,countofstr*sizeof(string_of_file));
			if (rarrofstr == NULL)
			{
				printf("Недостаточно памяти.\n");
				memory_err_flag = 1;
				return NULL;
			}
			else
			{
				arrofstr=rarrofstr;
				rarrofstr=NULL;
			}

			emptyfile = 0;

			if (arg1 != -1)
			{
				i = countofstr - 1;

				while (i != arg1)
				{
					arrofstr[i].str = arrofstr[i-1].str;
					arrofstr[i].stringlenght = arrofstr[i-1].stringlenght;

					i--;
				}

				arrofstr[i].str = NULL;

				arrofstr[i].str = insertstr.str;
				arrofstr[i].stringlenght = insertstr.stringlenght;

				insertstr.str = NULL;

				arg1++;
			}
			else
			{
				arrofstr[countofstr - 1].str = insertstr.str;
				arrofstr[countofstr - 1].stringlenght = insertstr.stringlenght;

				insertstr.str = NULL;
			}

			new_str_flag = 0;

			if (comand[k] == '\0')
			{
				printf("String was successfully inserted.\n");
				return arrofstr;
			}
		}
	}
	else

		/*МНОГОСТРОЧНЫЙ РЕЖИМ*/
	if (multi_str_flag == 1)      
	{
		while (1)
		{
			if (firstinflag == 0)
			{
				while (comand[i] != '\0')
				{
					if (comand[0] == '"')
					{
						printf("Неверные аргументы.\n");
						printf("Введите команду заново.\n");
						printf("Аргументами должны быть число больше или равное 0\nи строка, заключенная в двойные кавычки\nили несколько строк заключенные в тройные кавычки.\n");
						return arrofstr;
					}

					if ((i != 0) && (comand[i] == '"') && (comand[i - 1] != '\\') && (comand[i + 1] != '\0') && (comand[i + 1] != '"') && (comand[i + 2] != '\0') && (comand[i + 2] != '"'))
					{
						printf("Неверные аргументы.\n");
						printf("Введите команду заново.\n");
						printf("Аргументами должны быть число больше или равное 0\nи строка, заключенная в двойные кавычки\nили несколько строк заключенные в тройные кавычки.\n");
						return arrofstr;
					}

					if ((comand[i] == '\\') && (comand[i + 1] != 'n') && (comand[i + 1] != 't') && (comand[i + 1] != '\\') && (comand[i + 1] != '"') && (comand[i + 1] != 'r') && ((i != 0) && comand[i - 1] != '\\'))
					{	
						printf("Неверные аргументы.\n");
						printf("Введите команду заново.\n");
						printf("Аргументами должны быть число больше или равное 0\nи строка, заключенная в двойные кавычки\nили несколько строк заключенные в тройные кавычки.\n");
						return arrofstr;
					}

					i++;
				}

				if (i - 3 >= 0)
				{
					if ((comand[i - 1] == '"') && (comand[i - 2] == '"') && (comand[i - 3] == '"'))
					{
						if (i - 4 >= 0)
						{
							comand[i - 3] = '\0';
							multi_str_flag = 0;
						}
						else
						{
							printf("Неверные аргументы.\n");
							printf("Введите команду заново.\n");
							printf("Аргументами должны быть число больше или равное 0\nи строка, заключенная в двойные кавычки\nили несколько строк заключенные в тройные кавычки.\n");
							return arrofstr;
						}
					}
				}

				firstinflag = 1;
				i = 0;
			}

			i = k;

			insertstr.stringlenght = 0;

			while (comand[i] != '\0')
			{
				insertstr.stringlenght++;
				rptr = (char *)realloc(ptr,insertstr.stringlenght*sizeof(char));
				if (rptr == NULL)
				{
					free(ptr);
					ptr = NULL;
					printf("Недостаточно памяти.\n");
					memory_err_flag = 1;
					return NULL;
				}
				else
				{
					ptr = rptr;
					rptr = NULL;
				}

				if (comand[i] == '\\')
				{
					if (comand[i + 1] == 't')
					{
						ptr[insertstr.stringlenght - 1] = '\t';
						i++;
					}
					else
					if (comand[i + 1] == '\\')
					{
						ptr[insertstr.stringlenght - 1] = '\\';
						i++;
					}
					else
					if (comand[i + 1] == 'r')
					{
						ptr[insertstr.stringlenght - 1] = '\r';
						i++;
					}
					else
					if (comand[i + 1] == '"')
					{
						ptr[insertstr.stringlenght - 1] = '"';
						i++;
					}
					else
					if (comand[i + 1] == 'n')
					{
						ptr[insertstr.stringlenght-1] = '\n';
						i++;
						i++;
						new_str_flag = 1;
						break;
					}

				}
				else
				{
					ptr[insertstr.stringlenght-1] = comand[i];
				}

				i++;
			}

			k=i;

			if (new_str_flag == 0)
			{
				insertstr.stringlenght++;
				rptr = (char *)realloc(ptr,insertstr.stringlenght*sizeof(char));
				if (rptr == NULL)
				{
					free(ptr);
					ptr = NULL;
					printf("Недостаточно памяти.\n");
					memory_err_flag = 1;
					return NULL;
				}
				else
				{
					ptr = rptr;
					rptr = NULL;
				}
				ptr[insertstr.stringlenght - 1] = '\n';
			}

			insertstr.str = ptr;
			ptr = NULL;

			countofstr++;

			rarrofstr = (string_of_file *)realloc(arrofstr,countofstr*sizeof(string_of_file));
			if (rarrofstr == NULL)
			{
				printf("Недостаточно памяти.\n");
				memory_err_flag = 1;
				return NULL;
			}
			else
			{
				arrofstr = rarrofstr;
				rarrofstr = NULL;
			}

			emptyfile = 0;

			if (arg1 != -1)
			{
				i = countofstr - 1;

				while (i != arg1)
				{
					arrofstr[i].str = arrofstr[i-1].str;
					arrofstr[i].stringlenght = arrofstr[i-1].stringlenght;

					i--;
				}

				arrofstr[i].str = NULL;

				arrofstr[i].str = insertstr.str;
				arrofstr[i].stringlenght = insertstr.stringlenght;

				insertstr.str = NULL;

				arg1++;
			}
			else
			{
				arrofstr[countofstr-1].str = insertstr.str;
				arrofstr[countofstr-1].stringlenght = insertstr.stringlenght;

				insertstr.str = NULL;
			}

			if (multi_str_flag == 0)
			{
				printf("Strings were successfully inserted.\n");
				return arrofstr;
			}

			if (new_str_flag == 0)
			{
				while (1)
				{
					free(comand);
					comand = NULL;

					i = -1;
					k = 0;

					letter = (int)getchar();	
					while (letter != '\n')
					{
						if (((letter==' ')||(letter=='	'))&&(spaceflag==0))
						{
							i++;
							rptr = (char *)realloc(comand,(i+1)*sizeof(char));
							if (rptr == NULL)
							{
								printf("Недостаточно памяти.\n");
								memory_err_flag = 1;
								return NULL;
							}
							else
							{
								comand = rptr;
								rptr = NULL;
							}

							comand[i] = ' ';
							spaceflag = 1;
						}
						else
						if ((letter!=' ')&&(letter!='	'))
						{
							i++;
							rptr = (char *)realloc(comand,(i+1)*sizeof(char));
							if (rptr == NULL)
							{
								printf("Недостаточно памяти.\n");
								memory_err_flag = 1;
								return NULL;
							}
							else
							{
								comand = rptr;
								rptr = NULL;
							}
							comand[i] = letter;
							spaceflag = 0;
						}

						letter = (int)getchar();		
					}

					i++;
					rptr = (char *)realloc(comand,(i+1)*sizeof(char));
					if (rptr == NULL)
					{
						printf("Недостаточно памяти.\n");
						memory_err_flag = 1;
						return NULL;
					}
					else
					{
						comand = rptr;
						rptr = NULL;
					}
					comand[i] = '\0';

					if (i - 3 >= 0)
					{
						if ((comand[i - 1] == '"') && (comand[i - 2] == '"') && (comand[i - 3] == '"'))
						{
							if (i - 4 >= 0)
							{
								comand[i - 3] = '\0';
								multi_str_flag = 0;
								break;
							}
							else
							{
								printf("Strings were successfully inserted.\n");
								return arrofstr;
							}
						}
					}

					i = 0;

					while (comand[i] != '\0')
					{
						if (comand[0] == '"')
						{
							printf("Строка введена неверно.\n");
							printf("Введите строку заново.\n");
							i = -1;
							break;
						}

						if ((i != 0) && (comand[i] == '"') && (comand[i - 1] != '\\'))
						{
							printf("Строка введена неверно.\n");
							printf("Введите строку заново.\n");
							i=-1;
							break;
						}

						if ((comand[i] == '\\') && (comand[i + 1] != 'n') && (comand[i + 1] != 't') && (comand[i + 1] != '\\') 
							&& (comand[i + 1] != '"') && (comand[i + 1] != 'r') && ((i != 0) && comand[i - 1] != '\\'))
						{	
							printf("Строка введена неверно.\n");
							printf("Введите строку заново.\n");
							i = -1;
							break;
						}

						i++;
					}

					if (i != -1)
					{
						break;
					}
				}
			} /*if*/

			new_str_flag=0;
		} /*while*/
	} /*if*/
}

string_of_file *fdelete_range (string_of_file *arrofstr)
{
	int letter = 0, len = 0, i = 0, j = 0, l = 0, termstr1 = 0, countofstr1 = 0, arg1 = 0, arg2 = 0, argflag = 0;
	char *ptr = NULL;


	arg_separation();

	if (((comand[0] > '9') || (comand[0] < '1')) && (comand[0] != '\0'))
	{
		printf("Неверные аргументы.\n");
		printf("Введите команду заново.\n");
		printf("Аргументами должны быть числа больше 0,\nпричём первое число должно быть меньше второго.\n");
		return arrofstr;
	}

	while (comand[i] != '\0')
	{
		if (((comand[i] > '9') || (comand[i] < '0')) && (comand[i] != ' '))
		{
			printf("Неверные аргументы.\n");
			printf("Введите команду заново.\n");
			printf("Аргументами должны быть числа больше 0,\nпричём первое число должно быть меньше второго.\n");
			return arrofstr;
		}
		i++;
	}

	i = 0;

	if (comand[0] == '\0')
	{
		printf("Неверные аргументы.\n");
		printf("Введите команду заново.\n");
		printf("Аргументами должны быть числа больше 0,\nпричём первое число должно быть меньше второго.\n");
		return arrofstr;
	}
	else
	{
		arg1 = atoi(comand);

		while ((comand[i] != ' '))
		{
			if (comand[i] == '\0')
			{
				i = -1;
				break;
			}
			i++;
		}

		if (i != -1)
		{
			comandrest = i;
			arg_separation();
			arg2 = atoi(comand);
			argflag = 1;
		}

		if ((((arg1 > arg2) || (arg2 > countofstr)) && (argflag == 1)) || (arg1 > countofstr))
		{
			printf("Неверные аргументы.\n");
			printf("Введите команду заново.\n");
			printf("Аргументами должны быть числа больше 0,\nпричём первое число должно быть меньше второго.\n");
			return arrofstr;
		}
	}

	if (argflag == 0)
	{
		arg2 = countofstr;
	}


	for (i = (arg1 - 1); i < arg2; i++)
	{
		ptr = arrofstr[i].str;
		free(ptr);
		ptr = NULL;
		arrofstr[i].str = NULL;
	}

	i = arg1 - 1;
	j = arg2;

	while (j != countofstr)
	{
		arrofstr[i].str = arrofstr[j].str;
		arrofstr[i].stringlenght = arrofstr[j].stringlenght;

		i++;
		j++;
	}

	countofstr = countofstr - (arg2 - (arg1 - 1));

	printf("String/strings from %i to %i was/were deleted.\n", arg1, arg2);

	return arrofstr;
}

string_of_file *freplace_substring (string_of_file *arrofstr)
{
	int i = 0,j = 0,k = 0,l = 0,pos = 0,arg1 = 0,arg2 = 0,argflag = 0,str1len = 0,str2len = 0,len = 0,positionflag = 0,symbolflag = 0;
	char *strarg1 = NULL,*strarg2 = NULL,*ptr1 = NULL,*ptr2 = NULL,*rptr = NULL;
	string_of_file *rarrofstr = NULL;


	arg_separation();

	if (((comand[0]>'9') || (comand[0]<'1'))&&(comand[0] != '"'))
	{
		printf("Неверные аргументы.\n");
		printf("Введите команду заново.\n");
		printf("Аргументами должны быть два числа больше 0,\nпричём первое число должно быть меньше или равно второму,\nи две строки заключенный в кавычки.\n");
		return arrofstr;
	}

	
	if (comand[0] != '"')
	{
		arg1 = atoi(comand);
		argflag = 1;

		if ((arg1>countofstr) || (arg1<0))
		{
			printf("Неверные аргументы.\n");
			printf("Введите команду заново.\n");
			printf("Аргументами должны быть два числа больше 0,\nпричём первое число должно быть меньше или равно второму,\nи две строки заключенный в кавычки.\n");
			return arrofstr;
		}
	}
	else
	{
		arg1 = 1;
		arg2 = countofstr;
	}

	i = 0;

	if (argflag == 1)
	{
		while (comand[i] != ' ')
		{
			if ((comand[i] == '\0') || ((comand[0]>'9') || (comand[0]<'0')))
			{
				printf("Неверные аргументы.\n");
				printf("Введите команду заново.\n");
				printf("Аргументами должны быть два числа больше 0,\nпричём первое число должно быть меньше или равно второму,\nи две строки заключенный в кавычки.\n");
				return arrofstr;
			}

			i ++;
		}

		comandrest = i;
		arg_separation();

		if (comand[0] != '"')
		{
			arg2 = atoi(comand);
			argflag = 2;

			if ((arg2>countofstr) || (arg2<0) || (arg2<arg1))
			{
				printf("Неверные аргументы.\n");
				printf("Введите команду заново.\n");
				printf("Аргументами должны быть два числа больше 0,\nпричём первое число должно быть меньше или равно второму,\nи две строки заключенный в кавычки.\n");
				return arrofstr;
			}
		}
		else
		{
			arg2 = countofstr;
		}

		i = 0;

		if (argflag == 2)
		{
			while (comand[i] != ' ')
			{
				if ((comand[i] == '\0') || ((comand[0]>'9') || (comand[0]<'0')))
				{
					printf("Неверные аргументы.\n");
					printf("Введите команду заново.\n");
					printf("Аргументами должны быть два числа больше 0,\nпричём первое число должно быть меньше или равно второму,\nи две строки заключенный в кавычки.\n");
					return arrofstr;
				}

				i ++;
			}

			comandrest = i;
			arg_separation();
		}
	}

	if (comand[0] != '"')
	{
		printf("Неверные аргументы.\n");
		printf("Введите команду заново.\n");
		printf("Аргументами должны быть два числа больше 0,\nпричём первое число должно быть меньше или равно второму,\nи две строки заключенный в кавычки.\n");
		return arrofstr;
	}

	i = 0;
	j = 1;

	while (comand[j] != '\0')
	{
		comand[i] = comand[j];

		i ++;
		j ++;
	}
	comand[i] = '\0';

	if ((comand[0] == '"') || (comand[0] == '\0'))
	{
		printf("Неверные аргументы.\n");
		printf("Введите команду заново.\n");
		printf("Аргументами должны быть два числа больше 0,\nпричём первое число должно быть меньше или равно второму,\nи две строки заключенный в кавычки.\n");
		return arrofstr;
	}

	i = 0;
	j = 0;

	rptr = (char *)realloc(strarg1,(j + 1)*sizeof(char));
	if (rptr == NULL)
	{
		free(strarg1);
		strarg1 = NULL;
		printf("Недостаточно памяти.\n");
		memory_err_flag = 1;
		return NULL;
	}
	else
	{
		strarg1 = rptr;
		rptr = NULL;
	}

	if (comand[i] == '\\')
		{
			if (comand[i + 1] == 't')
			{
				strarg1[j] = '\t';
				i ++;
			}
			else
			if (comand[i + 1] == '\\')
			{
				strarg1[j] = '\\';
				i ++;
			}
			else
			if (comand[i + 1] == 'r')
			{
				strarg1[j] = '\r';
				i ++;
			}
			else
			if (comand[i + 1] == '"')
			{
				strarg1[j] = '"';
				i ++;
			}
			/*else
			if (comand[i + 1] == 'n')
			{
				strarg1[j] = '\n';
				i ++;
			}*/
			else
			{
				printf("Неверные аргументы.\n");
				printf("Введите команду заново.\n");
				printf("Аргументами должны быть два числа больше 0,\nпричём первое число должно быть меньше или равно второму,\nи две строки заключенный в кавычки.\n");
				free(strarg1);
				strarg1 = NULL;
				return arrofstr;
			}
		}
		else
		{
			strarg1[j] = comand[i];
		}

	i ++;
	j ++;

	while ((comand[i -1 ] == '\\') || (comand[i] != '"'))
	{
		rptr = (char *)realloc(strarg1,(j + 1)*sizeof(char));
		if (rptr == NULL)
		{
			free(strarg1);
			strarg1 = NULL;
			printf("Недостаточно памяти.\n");
			memory_err_flag = 1;
			return NULL;
		}
		else
		{
			strarg1 = rptr;
			rptr = NULL;
		}

		if (comand[i] == '\\')
		{
			if (comand[i + 1] == 't')
			{
				strarg1[j] = '\t';
				i ++;
			}
			else
			if (comand[i + 1] == '\\')
			{
				strarg1[j] = '\\';
				i ++;
			}
			else
			if (comand[i + 1] == 'r')
			{
				strarg1[j] = '\r';
				i ++;
			}
			else
			if (comand[i + 1] == '"')
			{
				strarg1[j] = '"';
				i ++;
			}
			/*else
			if (comand[i + 1] == 'n')
			{
				strarg1[j] = '\n';
				i ++;
			}*/
			else
			{
				printf("Неверные аргументы.\n");
				printf("Введите команду заново.\n");
				printf("Аргументами должны быть два числа больше 0,\nпричём первое число должно быть меньше или равно второму,\nи две строки заключенный в кавычки.\n");
				free(strarg1);
				strarg1 = NULL;
				return arrofstr;
			}
		}
		else
		{
			strarg1[j] = comand[i];
		}

		i ++;
		j ++;
	}

	rptr = (char *)realloc(strarg1,(j + 1)*sizeof(char));
	if (rptr == NULL)
	{
		free(strarg1);
		strarg1 = NULL;
		printf("Недостаточно памяти.\n");
		memory_err_flag = 1;
		return NULL;
	}
	else
	{
		strarg1 = rptr;
		rptr = NULL;
	}
	strarg1[j] = '\0';

	str1len = j + 1;

	if ((strarg1[0] == '^')&&(strarg1[1] == '\0'))
	{
		symbolflag = 1;
	}
	else
	if ((strarg1[0] == '$')&&(strarg1[1] == '\0'))
	{
		symbolflag = 2;
	}

	comandrest = i + 1;
	arg_separation();

	if (comand[0] != '"')
	{
		printf("Неверные аргументы.\n");
		printf("Введите команду заново.\n");
		printf("Аргументами должны быть два числа больше 0,\nпричём первое число должно быть меньше или равно второму,\nи две строки заключенный в кавычки.\n");
		free(strarg1);
		strarg1 = NULL;
		return arrofstr;
	}

	i = 0;
	j = 1;

	while (comand[j] != '\0')
	{
		comand[i] = comand[j];

		i ++;
		j ++;
	}
	comand[i] = '\0';

	if (i != 0)
	{
		if (comand[i -1 ] != '"')
		{
			printf("Неверные аргументы.\n");
			printf("Введите команду заново.\n");
			printf("Аргументами должны быть два числа больше 0,\nпричём первое число должно быть меньше или равно второму,\nи две строки заключенный в кавычки.\n");
			free(strarg1);
			strarg1 = NULL;
			return arrofstr;
		}
	}

	if ((comand[0] == '"') || (comand[0] == '\0'))
	{
		printf("Неверные аргументы.\n");
		printf("Введите команду заново.\n");
		printf("Аргументами должны быть два числа больше 0,\nпричём первое число должно быть меньше или равно второму,\nи две строки заключенный в кавычки.\n");
		free(strarg1);
		strarg1 = NULL;
		return arrofstr;
	}

	i = 0;
	j = 0;

	rptr = (char *)realloc(strarg2,(j + 1)*sizeof(char));
	if (rptr == NULL)
	{
		free(strarg2);
		strarg2 = NULL;
		free(strarg1);
		strarg1 = NULL;
		printf("Недостаточно памяти.\n");
		memory_err_flag = 1;
		return NULL;
	}
	else
	{
		strarg2 = rptr;
		rptr = NULL;
	}

	if (comand[i] == '\\')
		{
			if (comand[i + 1] == 't')
			{
				strarg2[j] = '\t';
				i ++;
			}
			else
			if (comand[i + 1] == '\\')
			{
				strarg2[j] = '\\';
				i ++;
			}
			else
			if (comand[i + 1] == 'r')
			{
				strarg2[j] = '\r';
				i ++;
			}
			else
			if (comand[i + 1] == '"')
			{
				strarg2[j] = '"';
				i ++;
			}
			/*else
			if (comand[i + 1] == 'n')
			{
				strarg2[j] = '\n';
				i ++;
			}*/
			else
			{
				printf("Неверные аргументы.\n");
				printf("Введите команду заново.\n");
				printf("Аргументами должны быть два числа больше 0,\nпричём первое число должно быть меньше или равно второму,\nи две строки заключенный в кавычки.\n");
				free(strarg1);
				strarg1 = NULL;
				free(strarg2);
				strarg2 = NULL;
				return arrofstr;
			}
		}
		else
		{
			strarg2[j] = comand[i];
		}


	i ++;
	j ++;

	while ((comand[i -1 ] == '\\') || (comand[i] != '"'))
	{
		rptr = (char *)realloc(strarg2,(j + 1)*sizeof(char));
		if (rptr == NULL)
		{
			free(strarg2);
			strarg2 = NULL;
			free(strarg1);
			strarg1 = NULL;
			printf("Недостаточно памяти.\n");
			memory_err_flag = 1;
			return NULL;
		}
		else
		{
			strarg2 = rptr;
			rptr = NULL;
		}

		if (comand[i] == '\\')
		{
			if (comand[i + 1] == 't')
			{
				strarg2[j] = '\t';
				i ++;
			}
			else
			if (comand[i + 1] == '\\')
			{
				strarg2[j] = '\\';
				i ++;
			}
			else
			if (comand[i + 1] == 'r')
			{
				strarg2[j] = '\r';
				i ++;
			}
			else
			if (comand[i + 1] == '"')
			{
				strarg2[j] = '"';
				i ++;
			}
			/*else
			if (comand[i + 1] == 'n')
			{
				strarg2[j] = '\n';
				i ++;
			}*/
			else
			{
				printf("Неверные аргументы.\n");
				printf("Введите команду заново.\n");
				printf("Аргументами должны быть два числа больше 0,\nпричём первое число должно быть меньше или равно второму,\nи две строки заключенный в кавычки.\n");
				free(strarg1);
				strarg1 = NULL;
				free(strarg2);
				strarg2 = NULL;
				return arrofstr;
			}
		}
		else
		{
			strarg2[j] = comand[i];
		}

		i ++;
		j ++;
	}
	rptr = (char *)realloc(strarg2,(j + 1)*sizeof(char));
	if (rptr == NULL)
	{
		free(strarg2);
		strarg2 = NULL;
		free(strarg1);
		strarg1 = NULL;
		printf("Недостаточно памяти.\n");
		memory_err_flag = 1;
		return NULL;
	}
	else
	{
		strarg2 = rptr;
		rptr = NULL;
	}
	strarg2[j] = '\0';
	
	str2len = j + 1;

	j = 0;
	pos = 0;

	for (i = (arg1 -1 );i<arg2;i ++)
	{
		pos = i + j;

		ptr1 = arrofstr[pos].str;
		len = arrofstr[pos].stringlenght;

		if (symbolflag == 0)
		{
			ptr1[len -1 ] = '\0';
			ptr2 = strstr(ptr1,strarg1);
			ptr1[len -1 ] = '\n';
		}
		else
		if (symbolflag == 1)
		{
			ptr2 = ptr1;
		}
		else
		if (symbolflag == 2)
		{
			ptr2 = &ptr1[len -1 ];
		}

		while (ptr2 != NULL)
		{
			k = 0;

			while (ptr2[k] != '\n')
			{
				k ++;
			}
			k ++;
			positionflag = len -k ;
			
			k = 0;

			while (strarg2[k] != '\n')
			{
				k ++;

				if (k == str2len -1 )
				{
					break;
				}
			}
			k ++;

			if (symbolflag == 0)
			{
				l = 0;

				while (ptr2[l + str1len -1 ] != '\n')
				{
					ptr2[l] = ptr2[l + str1len -1 ];
					l ++;
				}
				ptr2[l] = '\n';
			}

			if (symbolflag == 1)
			{
				l = len + str2len -1 ;
			}
			else
			{
				l = len - str1len + str2len;
			}

			rptr = (char *)realloc(ptr1,l*sizeof(char));
			if (rptr == NULL)
			{
				free(ptr1);
				ptr1 = NULL;
				free(strarg2);
				strarg2 = NULL;
				free(strarg1);
				strarg1 = NULL;
				printf("Недостаточно памяти.\n");
				memory_err_flag = 1;
				return NULL;
			}
			else
			{
				ptr1 = rptr;
				rptr = NULL;
			}

			len = l;

			while (l != (positionflag + str2len -1 ))
			{
				ptr1[l - 1] = ptr1[l - str2len];

				l --; 
			}

			for (l = 0;l<k -1 ;l ++)
			{
				ptr1[positionflag + l] = strarg2[l];
			}

			if (symbolflag == 2)
			{
				len ++;

				rptr = (char *)realloc(ptr1,len*sizeof(char));
				if (rptr == NULL)
				{
					free(ptr1);
					ptr1 = NULL;
					free(strarg2);
					strarg2 = NULL;
					free(strarg1);
					strarg1 = NULL;
					printf("Недостаточно памяти.\n");
					memory_err_flag = 1;
					return NULL;
				}
				else
				{
					ptr1 = rptr;
					rptr = NULL;
				}

				ptr1[len -1 ] = '\n';
			}

			rptr = &ptr1[positionflag + str2len -1 ];

			ptr1[len -1 ] = '\0';
			ptr2 = strstr(rptr,strarg1);
			ptr1[len -1 ] = '\n';

			arrofstr[pos].str = ptr1;
			arrofstr[pos].stringlenght = len;
		}
		ptr1 = NULL;
	}
	free(strarg1);
	strarg1 = NULL;
	free(strarg2);
	strarg2 = NULL;

	printf("Substrings were successfully replaced.\n");

	return arrofstr;
}