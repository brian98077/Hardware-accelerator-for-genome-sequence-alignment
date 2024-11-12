CC = g++
CFLAGS = -c
OPTFLAGS = -O2

bin/wga_final	:  wga_final.o lib
			$(CC) $(OPTFLAGS)  wga_final.o -ltm_usage -Llib -o bin/wga_final
wga_final.o 	   	: src/wga_final.cpp lib/tm_usage.h
			$(CC) $(CFLAGS) $< -Ilib -o $@