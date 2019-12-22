#include "Redactor.h"

char *toram (FILE *f)
{
	charnum = 0;
	int stepram = 1;
	int letter = ' '; 
	char *ptr = NULL,*rptr = NULL;


	end_flag = 0;
	ptr = (char *)malloc(STRLEN*sizeof(char));
	if (ptr == NULL)
	{
		printf("Недостаточно памяти.\n");
		memory_err_flag=1;
		return ptr;
	}
	
	while (letter != '\n')
	{
		letter = (char)fgetc(f); 

		if (feof(f))
		{
			end_flag=1;
			break;
		}

		emptyfile = 0;

		ptr[charnum]=(char)letter;
		charnum++;
		
		if (charnum > stepram * STRLEN - 1)
		{
			stepram++;
			rptr=(char *)realloc(ptr,stepram*STRLEN*sizeof(char));
			if (rptr == NULL)
			{
				free(ptr);
				ptr = NULL;
				printf("Недостаточно памяти.\n");
				memory_err_flag=1;
				return rptr;
			}
			else
			{
				ptr = rptr;
				rptr = NULL;
			}
		}	
	}
	return ptr;
} 

string_of_file *file_toram (FILE *f)
{
	string_of_file *arrofstr = NULL, *arrofstr2 = NULL;
	string_of_file str2;
	char *ptr=NULL;


	/*СОЗДАНИЕ МАССИВА СТРУКТУР*/
	
	while (1) 
	{ 
		ptr = toram(f);
		
		if (end_flag == 1)
		{
			free(ptr);
			ptr = NULL;
			break;
		}
		else
		{
			str2.str = ptr;
			str2.stringlenght = charnum;
			ptr=NULL;

			countofstr++;

			arrofstr2 = (string_of_file *)realloc(arrofstr, countofstr*sizeof(string_of_file));
			if (arrofstr2==NULL)
			{
				printf("Недостаточно памяти.\n");
				memory_err_flag=1;
				return arrofstr2;
			}
			else
			{
				arrofstr = arrofstr2;
				arrofstr2 = NULL;
			}

			arrofstr[countofstr - 1].str = str2.str;
			arrofstr[countofstr - 1].stringlenght=str2.stringlenght;
		}
	}
	return arrofstr;
}