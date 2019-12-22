CC = gcc
#CFLAGS = -Werror -Wall -Wextra  -g
CFLAGS = -Wall -Wextra  -g

FI = newRedactor.c filetoram.c analys_comands.c prints_commands.c technical_comands.c redact_str.c

Redactor: $(patsubst %.c, %.o, $(FI))
	$(CC) $(CFLAGS) -o $@ $^

%.c : Redactor.h

%.o : %.c
	$(CC) $(CFLAGS) -c $^

clean:
	rm -f Redactor *.o
