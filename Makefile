CC     := g++                              
CFLAGS 	= -O3 -ansi              
#CFLAGS 	= -Wall -g               
INC 	= -I ./include 
LIB 	= -L ./lib  
OBJS 	= main.o malloc.o
#OBJS 	= main.o
EXE 	= main

#-----------------------------------------------------------------------------
# clear suffix list and set new one
.SUFFIXES:
.SUFFIXES: .c .cpp .o
#-----------------------------------------------------------------------------

${EXE}: ${OBJS}
	${CC} ${OBJS} ${CFLAGS} ${INC} ${LIB} -o $@ 
	@rm -rf *.o 
%.o: %.c 
	${CC} $< ${CFLAGS} ${INC} ${LIB} -lpthread -c

.PHONY: clean
clean:
	@rm -rf *.o
	@rm -rf main
