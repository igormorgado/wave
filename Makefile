LIBS = -lm 
CFLAGS = -Wall -g -ggdb 
# -pg
OBJS = tic ricker utils simulation velocity_model globals wavefield
UTILS = bfdiff bddiff d2f d2a
PROGS = wave oldwave
TESTS = test_tic test_ricker
CC = gcc

SRC=$(wildcard *.c)

all: $(OBJS) $(UTILS) $(PROGS)

wave: $(OBJS) 
	$(CC) -c wave.c $(CFLAGS) 
	$(CC) -o wave wave.o wavefield.o globals.o ricker.o tic.o simulation.o velocity_model.o $(CFLAGS) $(LIBS)

tests: $(TESTS)


##################################################
## LIBS
##################################################

objs: $(OBJS)

globals:
	$(CC) -c globals.c $(CFLAGS)

ricker:
	$(CC) -c ricker.c $(CFLAGS) $(LIBS)

simulation:
	$(CC) -c simulation.c $(CFLAGS)

velocity_model:
	$(CC) -c velocity_model.c $(CFLAGS)

tic:
	$(CC) -c tic.c $(CFLAGS)

utils:
	$(CC) -c utils.c $(CFLAGS)

wavefield:
	$(CC) -c wavefield.c $(CFLAGS) $(LIBS)


##################################################
## TESTS
##################################################

tests: objs $(TESTS)

test_tic: tic
	$(CC) -o test_tic test_tic.c tic.o $(LIBS) $(CFLAGS)

test_ricker: ricker
	$(CC) -o test_ricker test_ricker.c ricker.o $(CFLAGS) $(LIBS)


##################################################
## UTILS 
##################################################

bfdiff: utils
	$(CC) -o bfdiff bfdiff.c utils.o $(CFLAGS) 

bddiff: utils
	$(CC) -o bddiff bddiff.c utils.o $(CFLAGS) 

d2f:
	$(CC) -o d2f d2f.c $(CFLAGS) 

d2a:
	$(CC) -o d2a d2a.c $(CFLAGS) 

oldwave:
	$(CC) -o oldwave oldwave.c $(CFLAGS) $(LIBS)

oldwavew:
	$(CC) -o oldwavew oldwavew.c $(CFLAGS) $(LIBS)

oldwavef:
	$(CC) -o oldwavef oldwavef.c $(CFLAGS) $(LIBS)

##################################################
## Clean all 
##################################################
clean: 
	rm -f $(PROGS) $(TESTS) $(UTILS) a.out *.o 

