#include "Redactor.h"

void fprint_pages (string_of_file *arrofstr);

void arg_separation ()
{
	int i = 0, j = 0;

	i = comandrest;
	while (comand[i] != '\0')
	{
		comand[j] = comand[i];
		j++;
		i++;
	}	
	comand[j] = '\0';


	i = 0;
	while (comand[i] != '\0')
	{
		if (comand[i] == '#')
		{
			comand[i] = '\0';
			break;
		}
		i++;
	}

	if (comand[0] != '\0')
	{
		j = 1;
		while (comand[j-1] != '\0')
		{
			comand[j-1] = comand[j];
			j++;
		}
	}

	if (comand[0] != '\0')
	{
		j = j-2;
		if (comand[j-1] == ' ')
			while (comand[j-1] != '\0')
			{
				comand[j-1] = comand[j];
				j++;
			}
	}

	return;
}

int cmpstr (char *str1, char *str2)
{
	int eos = 0;


	while (str1[eos] != '\0')
	{
		if (str1[eos] != str2[eos])
			return 1;
		else
		eos++;
	}
	if (str2[eos] == '\0') {
		return 0;
	}
	return 1;
}

void fexit (string_of_file *arrofstr)
{
	int i = 0;
	char exit_force[] = "force";

	if (memory_err_flag == 0)
		arg_separation();

	if (((cmpstr(comand,exit_force)==0)||((save_file_flag==1)&&(comand[0]=='\0')))||(memory_err_flag==1))
	{
		for (i = 0;i < countofstr; i++)
		{
			free(arrofstr[i].str);
			arrofstr[i].str = NULL;
		}
		
		if (emptyfile == 0)
		{
			free(arrofstr);
			arrofstr = NULL;
		}

		fclose(infile);
		infile = NULL;

		free(comand);
		comand = NULL;

		free(filename);
		filename = NULL;

		exitflag = 1;

		if (memory_err_flag == 0)
			printf("BYE-BYE \n");
		else
		if (memory_err_flag == 1)
		{
			printf("BYE-BYE (err)\n");
		}

		return;
	}
	else
	if ((comand[0] == '\0') && (save_file_flag == 0))
	{
		printf("Файл не сохранен.\n");
		printf("Используйте команду 'write', чтобы сохранить файл,\nи команду 'exit force', чтобы выйти без сохранения.\n");
		return;
	}
	else
	{
		printf("Неверный аргумент.\n");
		printf("Введите команду заново.\n");
		printf("Аргументом должно быть слово 'force'\nили аргументов не должно быть совсем\n");
		return;
	}
}

void comand_analize (string_of_file *arrofstr)
{
	int letter = 0, i = -1, j = 0, spaceflag = 0, errflag = 0, end_of_func_flag = 0, 
	freeflag = 0;
	string_of_file *arrofstr1 = NULL;
	char *rptr = NULL;
	
	/*МАССИВЫ КОМАНД*/
	char print_pages[] = "print pages";
	char print_range[] = "print range";
	char set_tabwidth[] = "set tabwidth";
	char set_wrap[] = "set wrap";
	char insert_after[] = "insert after";
	char edit_string[] = "edit string";
	char insert_symbol[] = "insert symbol";
	char replace_substring[] = "replace substring";                  
	char delete_range[] = "delete range";
	char exitminivim[] = "exit";
	char openfilewosave[] = "read";
	char openfilewsave[] = "open";
	char filesave[] = "write";
	char set_name[] = "set name";
	char help[] = "help";


	while (1)
	{
		if (exitflag == 1)
		return;
		if ((end_of_func_flag == 1) && (freeflag == 0))
		{
			free(comand);
			comand = NULL;
		}

		spaceflag = 0;
		errflag = 0;
		letter = 0;
		for (i = 0;i < COMANDLEN + 2; i++)
			comand_buf[i] = 0;
		i = -1;
		j = 0;
		end_of_func_flag = 0;
		comand_read_flag=0;
		freeflag = 0;

		/*ЧТЕНИЕ КОМАНДЫ*/
		printf(">Введите команду.\n");         
		printf(">");

		letter = (int)getchar();	
		while ((letter==' ')||(letter=='	'))
		{
			letter = (int)getchar();
		}
	
		while (letter != '\n')
		{
			if (((letter==' ')||(letter=='	'))&&(spaceflag == 0))
			{
				i++;
				rptr = (char *)realloc(comand,(i+1)*sizeof(char));
				if (rptr == NULL)
				{
					printf("Недостаточно памяти.\n");
					memory_err_flag = 1;
					return;
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
					return ;
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
			return;
		}
		else
		{
			comand = rptr;
			rptr = NULL;
		}
		comand[i] = '\0';
		i = 0;

		for (j=0; j<2; j++)
		{
			if (end_of_func_flag == 1)
			{
				free(comand);
				comand = NULL;
				freeflag = 1;
				break;
			}

			if (spaceflag == 2)
			{
				free(comand);
				comand = NULL;
				freeflag = 1;
				errflag = 1;
			}

			if (j == 1)
			{
				comand_read_flag = 1;
				if (spaceflag == 1)
				{
					comand_buf[i] = ' ';
					i++;
					if (i > COMANDLEN)
					{
						errflag = 1;
						break;
					}
				}
			}

			if ((errflag != 1) || (spaceflag != 2))
			{	
				if (comand[0] == '\0')
				{
					comand_buf[0] = '\0';
					spaceflag = 2;
					break;
				}
				else
				while ((comand[i] != ' '))
				{
					comand_buf[i] = comand[i];
					i++;

					if (comand[i] == '\0')
					{
						comand_buf[i] = '\0';
						spaceflag = 2;
						break;
					}
					else
					if (comand[i] == ' ')
					{
						comand_buf[i] = '\0';
						spaceflag = 1;
					}

					if (i > COMANDLEN)
					{
						errflag = 1;
						break;
					}
				}
			}

			if (errflag == 1)
			{
				printf("Такой команды не существует.\n");
				printf("Введите команду заново.\n");
				break;
			}
			else
			if (errflag != 1)
			{
				/*АНАЛИЗ НА СООТВЕТСВИЕ КОМАНДЫ*/
				if (cmpstr(comand_buf, print_pages) == 0)                     
				{
					comandrest = i;

					fprint_pages(arrofstr);

					if (memory_err_flag == 1)
					{
						fexit(arrofstr);
						break;
					}

					end_of_func_flag = 1;                                       
				} else 
				if (cmpstr(comand_buf, openfilewosave) == 0)
				{
					comandrest = i;

					arrofstr1 = freadfile(arrofstr);

					if ((arrofstr1 != NULL) || (((arrofstr1 == NULL) && (emptyfile == 1))))
					{
						arrofstr = arrofstr1;
					}
					arrofstr1 = NULL;

					if (memory_err_flag == 1)
					{
						fexit(arrofstr);
						break;
					}

					end_of_func_flag = 1;                                       
				} else
				if (cmpstr(comand_buf,openfilewsave)==0)
				{
					comandrest = i;

					arrofstr1 = fopenfile(arrofstr);

					if ((arrofstr1 != NULL) || (((arrofstr1 == NULL) && (emptyfile == 1))))
					{
						arrofstr = arrofstr1;
					}
					arrofstr1 = NULL;

					if (memory_err_flag == 1)
					{
						fexit(arrofstr);
						break;
					}

					end_of_func_flag = 1;                                      
				} else
				if (cmpstr(comand_buf,filesave) == 0)
				{
					comandrest = i;
					ffilesave(arrofstr);

					if (memory_err_flag == 1)
					{
						fexit(arrofstr);
						break;
					}

					end_of_func_flag = 1;                                       
				} else 
				if (cmpstr(comand_buf,exitminivim) == 0)
				{
					comandrest = i;
					fexit(arrofstr);

					if (exitflag == 1)
						break;

					end_of_func_flag = 1;                                       
				} else 
				if (cmpstr(comand_buf,set_name)==0)
				{
					comandrest = i;
					fset_name();

					if (memory_err_flag == 1)
					{
						fexit(arrofstr);
						break;
					}

					end_of_func_flag = 1;                                      
				} else
				if (cmpstr(comand_buf,set_tabwidth)==0)
				{
					comandrest = i;
					fset_tabwidth();

					if (settabwidthflag == -1)
					{
						settabwidthflag = 8;
					}

					if (memory_err_flag == 1)
					{
						fexit(arrofstr);
						break;
					}

					end_of_func_flag = 1;                                       
				} else 
				if (cmpstr(comand_buf,set_wrap) == 0)
				{
					comandrest = i;
					fset_wrap();

					if (setwrapflag == -1)
					{
						setwrapflag = 1;
					}

					if (memory_err_flag == 1)
					{
						fexit(arrofstr);
						break;
					}

					end_of_func_flag = 1;                                       
				} else 
				if (cmpstr(comand_buf, print_range) == 0)
				{
					comandrest = i;
					fprint_range(arrofstr);

					if (memory_err_flag == 1)
					{
						fexit(arrofstr);
						break;
					}

					end_of_func_flag = 1;                                            
				} else 
				if (cmpstr(comand_buf,edit_string)==0)
				{
					comandrest = i;
					arrofstr = fedit_string(arrofstr);

					if (memory_err_flag == 1)
					{
						fexit(arrofstr);
						break;
					}

					end_of_func_flag = 1;                                      
				} else
				if (cmpstr(comand_buf, insert_symbol) == 0)
				{
					comandrest = i;
					arrofstr = finsert_symbol(arrofstr);

					if (memory_err_flag == 1)
					{
						fexit(arrofstr);
						break;
					}

					end_of_func_flag = 1;                                       
				} else
				if (cmpstr(comand_buf, insert_after) == 0)
				{
					comandrest = i;
					arrofstr = finsert_after(arrofstr);

					if (memory_err_flag == 1)
					{
						fexit(arrofstr);
						break;
					}

					end_of_func_flag = 1;                                       
				} else 
				if (cmpstr(comand_buf,delete_range) == 0)
				{
					comandrest = i;
					arrofstr = fdelete_range(arrofstr);

					if (memory_err_flag == 1)
					{
						fexit(arrofstr);
						break;
					}

					end_of_func_flag = 1;                                       
				} else
				if (cmpstr(comand_buf,replace_substring) == 0)
				{
					comandrest = i;
					arrofstr = freplace_substring(arrofstr);

					if (memory_err_flag == 1)
					{
						fexit(arrofstr);
						break;
					}

					end_of_func_flag = 1;                                       
				} else
				if (cmpstr(comand_buf,help) == 0)
				{
					comandrest = i;
					fhelp();

					if (memory_err_flag == 1)
					{
						fexit(arrofstr);
						break;
					}

					end_of_func_flag = 1;                                      
				} else
				if ((comand_read_flag == 1) && (end_of_func_flag == 0))
				{
					printf("Такой команды не существует.\n");
					printf("Введите команду заново.\n");
				}
			}
		}
	}
}