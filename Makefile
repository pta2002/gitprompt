gitprompt: gitprompt.c
	gcc -Ofast -o gitprompt gitprompt.c -lgit2 -I.

run: gitprompt
	./gitprompt
