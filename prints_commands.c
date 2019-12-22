#include "Redactor.h"

int fset_tabwidth ()
{
	int i = 0;

	arg_separation();

	if ((comand[0] > '9') || (comand[0] < '1') || (comand[0] == '\0'))
	{
		printf("Неверный аргумент, выставлено значение по умолчанию.\n");
		printf("Введите команду заново.\n");
		printf("Аргументом должно быть число больше 0\n");
		settabwidthflag=-1;
		return settabwidthflag;
	}

	while (comand[i] != '\0')
	{
		if ((comand[i] > '9') || (comand[i] < '0'))
		{
			printf("Неверный аргумент, выставлено значение по умолчанию.\n");
			printf("Введите команду заново.\n");
			printf("Аргументом должно быть число больше 0\n");
			settabwidthflag = -1;
			return settabwidthflag;
		}
		i++;
	}

	settabwidthflag = atoi(comand);

	printf("Tabulations will be replaced with %i spaces.\n", settabwidthflag);

	return settabwidthflag;
}

int fset_wrap ()
{
	char set_wrap_yes[] = "yes";
	char set_wrap_no[] = "no";


	arg_separation();

	if (cmpstr(comand, set_wrap_yes) == 0)
	{
		setwrapflag = 1;
		printf("Strings are wraping.\n");
		return setwrapflag;
	}
	else
	if (cmpstr(comand, set_wrap_no) == 0)
	{
		setwrapflag = 0;
		printf("Strings aren't wraping.\n");
		return setwrapflag;
	}
	else
	{
		printf("Неверный аргумент.\n");
		printf("Введите команду заново.\n");
		printf("Аргументом должно быть слово 'yes' или слово 'no'\n");
		setwrapflag = -1;
		return setwrapflag;
	}
}

void fprint_pages (string_of_file *arrofstr)
{
	int letter = 0, len = 0, i = 0, j = 0, k = 0, l = 0, termstr1 = 0,
	countofstr1 = 0, printstep = 0, end_of_func_flag = 0, wrapflag = 0, errflag = 0,
	stringwrapflag = 0, endofstrflag = 0, shiftflag = 0, printflag = 0;
	char *ptr  = NULL;
	struct winsize term;


	ioctl(0,TIOCGWINSZ,&term);
	termstr = term.ws_row;
	termwid = term.ws_col;

	arg_separation();

	if (comand[0] == '\0')
	{
		termstr1 = termstr - 1;
		countofstr1 = countofstr;

		if (countofstr1 < termstr1)
		{
			termstr1 = countofstr1;
		}

		while (1)
		{
			printflag = 0;

			for (i = 0;i < termstr1; i++)
			{
				len = arrofstr[i + printstep].stringlenght;
				ptr = arrofstr[i + printstep].str;

				letter = 0;
				endofstrflag = 0;

				if ((setwrapflag == 0) && (termwid < len))
				{
					len = termwid;
					wrapflag = 1;

					if ((shiftflag > 0) && (len + shiftflag <= arrofstr[i+printstep].stringlenght))
					{
						len = len + shiftflag;
						if (len == arrofstr[i+printstep].stringlenght)
						{
							printflag = 1;
						}
					}
					else
					if ((shiftflag > 0) && (len + shiftflag > arrofstr[i+printstep].stringlenght))
					{
						len = arrofstr[i+printstep].stringlenght;
						printflag = 1;
					}
				}

				if ((setwrapflag == 1) && (termwid < len))
				{
					for (j = 0;j < len; j++)
					{
						if (ptr[j]=='	')
							letter = letter + settabwidthflag;
						else
							letter++;
					}

					while (letter > termwid)
					{
						letter = letter - termwid;
						l++;
					}

					stringwrapflag = 1;
				}

				letter = 0;

				if ((stringwrapflag == 1) && ((i + l + 1) >= termstr))
					break;

				for (j = shiftflag;j < len; j++)
				{
					if (ptr[j]=='	')
					{
						for (k = 0;k < settabwidthflag; k++)
						{
							letter++;

							if ((setwrapflag == 0) && (letter == len))
							{
								endofstrflag = 1;
								break;
							}

							putchar(' ');
						}
					}
					else
					{
						letter++;

						if ((setwrapflag == 0) && (letter == len))
						{
							endofstrflag=1;
							break;
						}

						putchar(ptr[j]);
					}

					if (endofstrflag == 1)
						break;
				}

				if (((wrapflag == 1) || (endofstrflag == 1)) && (printflag == 0))
				{
					putchar('\n');
					wrapflag = 0;
				}
			}

			if ((countofstr1-i) <= 0)
			{
				end_of_func_flag = 1;
				break;
			}

			printflag = 0;

			if (end_of_func_flag != 1)              /*ОБРАБОТЧИК ОПЕРАЦИЙ*/
				while (1)
				{
					printf(">");
					letter = getchar();

					if (letter == 'q')
					{
						end_of_func_flag = 1;

						while (letter != '\n')
						{
							letter = getchar();
							if (letter != '\n')
								errflag = 1;
						}

						if (errflag != 1)
							break;
					}
					else
					if (letter==' ')
					{
						while (letter != '\n')
						{
							letter = getchar();
							if (letter != '\n')
								errflag = 1;
						}

						if (errflag != 1)
							break;

						shiftflag = 0;
					}
					else
					if ((setwrapflag == 0) && (letter=='>'))
					{
						while (letter != '\n')
						{
							letter = getchar();
							if (letter != '\n')
								errflag = 1;
						}

						if (errflag == 0)
						{
							shiftflag++;
							break;
						}
					}
					else
					if ((setwrapflag == 0) && (letter=='<'))
					{
						while (letter != '\n')
						{
							letter = getchar();
							if (letter != '\n')
								errflag = 1;
						}

						if ((errflag == 0) && (shiftflag != 0))
						{
							shiftflag--;
							printflag = 1;
							break;
						}
						else
						{
							errflag = 1;
						}
					}
					else
					{
						errflag = 2;
					}

					if (errflag != 0)
					{
						printf("Такой операции не существует.\n");
						printf("Введите операцию заново.\n");
						if (errflag == 2)
							while (letter != '\n')
								letter = getchar();

						errflag = 0;
						end_of_func_flag = 0;
					}
				}

			if ((shiftflag == 0 ) && (printflag == 0))
			{
				countofstr1 = countofstr1 - i;
				printstep = printstep + i;

				l = 0;
				stringwrapflag = 0;

				if (countofstr1 < termstr1)
				{
					termstr1 = countofstr1;
				}

				if (termstr1 <= 0)
				{
					end_of_func_flag = 1;
					break;
				}
			}

			if (end_of_func_flag == 1)
			{
				break;
			}
		}
		ptr = NULL;
		return;
	}
	else
	{
		printf("Такой команды не существует.\n");
		printf("Введите команду заново.\n");
		return;
	}
}

void fprint_range (string_of_file *arrofstr)
{
	int letter = 0, len = 0, i = 0, j = 0, l = 0, termstr1 = 0, countofstr1 = 0, k = 0, printstep = 0, 
	end_of_func_flag = 0, arg1 = 0, arg2 = 0,argflag = 0, wrapflag = 0, 
	errflag = 0, stringwrapflag = 0, endofstrflag = 0, shiftflag = 0, printflag = 0;
	char *ptr = NULL;
	struct winsize term;


	ioctl(0, TIOCGWINSZ, &term);
	termstr = term.ws_row;
	termwid = term.ws_col;

	arg_separation();

	if (((comand[0] > '9') || (comand[0] < '1')) && (comand[0] != '\0'))
	{
		printf("Неверные аргументы.\n");
		printf("Введите команду заново.\n");
		printf("Аргументами должны быть числа больше 0,\nпричём первое число должно быть меньше второго.\n");
		return;
	}

	while (comand[i] != '\0')
	{
		if (((comand[i] > '9') || (comand[i] < '0')) && (comand[i]!=' '))
		{
			printf("Неверные аргументы.\n");
			printf("Введите команду заново.\n");
			printf("Аргументами должны быть числа больше 0,\nпричём первое число должно быть меньше второго.\n");
			return;
		}
		i++;
	}

	i = 0;

	if (comand[0] == '\0')
	{
		fprint_pages(arrofstr);
		return;
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
			return;
		}

		arg1--;

		termstr1 = termstr - 1;
		
		if (argflag == 1)
			countofstr1 = arg2 - arg1;
		else
			countofstr1 = countofstr - arg1;

		if (countofstr1 < 1)
		{
			printf("Неверные аргументы.\n");
			printf("Введите команду заново.\n");
			printf("Аргументами должны быть числа больше 0,\nпричём первое число должно быть меньше второго.\n");
			return;
		}

		if (countofstr1 < termstr1)
		{
			termstr1 = countofstr1;
		}

		while (1)
		{
			printflag = 0;

			for (i = 0;i < termstr1; i++)
			{
				len = arrofstr[i+printstep+arg1].stringlenght;
				ptr = arrofstr[i+printstep+arg1].str;

				letter = 0;
				endofstrflag = 0;

				if ((setwrapflag == 0) && (termwid < len))
				{
					len = termwid;
					wrapflag = 1;

					if ((shiftflag > 0) && (len + shiftflag <= arrofstr[i + printstep + arg1].stringlenght))
					{
						len = len + shiftflag;
						if (len == arrofstr[i + printstep + arg1].stringlenght)
						{
							printflag = 1;
						}
					}
					else
					if ((shiftflag > 0) && (len + shiftflag > arrofstr[i + printstep + arg1].stringlenght))
					{
						len = arrofstr[i + printstep + arg1].stringlenght;
						printflag = 1;
					}
				}

				if ((setwrapflag == 1) && (termwid < len))
				{
					for (j = 0; j < len; j++)
					{
						if (ptr[j]=='	')
							letter = letter + settabwidthflag;
						else
							letter++;
					}

					while (letter > termwid)
					{
						letter = letter - termwid;
						l++;
					}

					stringwrapflag = 1;
				}

				letter = 0;

				if (((i + l + 1) >= termstr) && (stringwrapflag == 1))
					break;

				for (j = shiftflag;j < len;j++)
				{
					if (ptr[j]=='	')
					{
						for (k = 0;k < settabwidthflag; k++)
						{
							letter++;

							if ((setwrapflag == 0) && (letter == len))
							{
								endofstrflag = 1;
								break;
							}

							putchar(' ');
						}
					}
					else
					{
						letter++;

						if ((setwrapflag == 0) && (letter == len))
						{
							endofstrflag = 1;
							break;
						}

						putchar(ptr[j]);
					}

					if (endofstrflag == 1)
						break;
				}

				if (((wrapflag == 1) || (endofstrflag == 1)) && (printflag == 0))
				{
					putchar('\n');
					wrapflag = 0;
				}
			}

			if ((countofstr1 - i) <= 0)
			{
				end_of_func_flag = 1;
				break;
			}

			printflag = 0;

			if (end_of_func_flag != 1)/*ОБРАБОТЧИК ОПЕРАЦИЙ*/
				while (1)
				{
					printf(">");
					letter = getchar();

					if (letter == 'q')
					{
						end_of_func_flag = 1;

						while (letter != '\n')
						{
							letter = getchar();
							if (letter != '\n')
								errflag = 1;
						}

						if (errflag != 1)
							break;
					}
					else
					if (letter == ' ')
					{
						while (letter != '\n')
						{
							letter = getchar();
							if (letter != '\n')
								errflag = 1;
						}

						if (errflag != 1)
							break;

						shiftflag = 0;
					}
					else
					if ((setwrapflag == 0) && (letter == '>'))
					{
						while (letter != '\n')
						{
							letter = getchar();
							if (letter != '\n')
								errflag = 1;
						}

						if (errflag == 0)
						{
							shiftflag++;
							break;
						}
					}
					else
					if ((setwrapflag == 0) && (letter == '<'))
					{
						while (letter != '\n')
						{
							letter = getchar();
							if (letter != '\n')
								errflag = 1;
						}

						if ((errflag == 0) && (shiftflag != 0))
						{
							shiftflag--;
							printflag = 1;
							break;
						}
						else
						{
							errflag = 1;
						}
					}
					else
					{
						errflag = 2;
					}

					if (errflag != 0)
					{
						printf("Такой операции не существует.\n");
						printf("Введите операцию заново.\n");
						if (errflag == 2)
							while (letter != '\n')
								letter=getchar();

						errflag = 0;
						end_of_func_flag = 0;
					}
				}

			if ((shiftflag == 0) && (printflag == 0))
			{
				countofstr1 = countofstr1 - i;
				printstep = printstep + i;

				l = 0;
				stringwrapflag = 0;

				if (countofstr1 < termstr1)
				{
					termstr1 = countofstr1;
				}

				if (termstr1 <= 0)
				{
					end_of_func_flag = 1;
					break;
				}
			}

			if (end_of_func_flag == 1)
			{
				break;
			}
		}
		ptr = NULL;
		return;
	}
}