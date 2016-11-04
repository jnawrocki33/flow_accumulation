
## Compilation flags
##comment out one or the other 
##debugging 
#CFLAGS = -g 

##release
CFLAGS = -O3 -DNDEBUG

CFLAGS+= -Wall



CC = g++ $(CFLAGS)


PROGS = flow

default: $(PROGS)

flow: fd_and_fa.cpp
	$(CC) fd_and_fa.cpp -o flow

clean::	
	rm *.o
	rm flow