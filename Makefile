gitprompt: gitprompt.c config.h themes/*
	gcc -Ofast -o gitprompt gitprompt.c -I.

config.h: config.def.h
	cp config.def.h config.h

run: gitprompt
	./gitprompt
