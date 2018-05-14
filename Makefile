LIBS = -lm 
CFLAGS = -Wall -g -ggdb 
# -pg
OBJS = tic ricker utils simulation velocity_model globals wavefield help parse
UTILS = bfdiff bddiff d2f d2a
PROGS = wave oldwave
TESTS = test_tic test_ricker test_velocity_model test_sub_model
CC = gcc

SRC=$(wildcard *.c)

all: $(OBJS) $(UTILS) $(PROGS) $(TESTS)

wave: $(OBJS) 
	$(CC) -c wave.c $(CFLAGS) 
	$(CC) -o wave wave.o help.o wavefield.o globals.o ricker.o tic.o simulation.o velocity_model.o parse.o $(CFLAGS) $(LIBS)

tests: $(TESTS)


##################################################
## LIBS
##################################################

objs: $(OBJS)

parse:
	gcc -c parse.c $(CFLAGS) $(LIBS)

help:
	gcc -c help.c $(CFLAGS) $(LIBS)

globals:
	$(CC) -c globals.c $(CFLAGS)

ricker:
	gcc -c ricker.c $(CFLAGS)

simulation:
	gcc -c simulation.c $(CFLAGS)

velocity_model: globals
	gcc -c velocity_model.c ${CFLAGS} $(LIBS)

tic:
	gcc -c tic.c $(CFLAGS)

utils:
	gcc -c utils.c $(CFLAGS)

wavefield:
	gcc -c wavefield.c $(CFLAGS) $(LIBS)


##################################################
## TESTS
##################################################

tests: objs $(TESTS)

test_tic: tic
	$(CC) -o test_tic test_tic.c tic.o $(LIBS) $(CFLAGS)

test_ricker: ricker
	$(CC) -o test_ricker test_ricker.c ricker.o $(CFLAGS) $(LIBS)

test_velocity_model: velocity_model globals
	$(CC) -o test_velocity_model test_velocity_model.c velocity_model.o globals.o $(CFLAGS) $(LIBS)

test_sub_model: velocity_model globals
	$(CC) -o test_sub_model test_sub_model.c velocity_model.o globals.o $(CFLAGS) $(LIBS)

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


##################################################
## Clean all 
##################################################
clean: 
	rm -f $(PROGS) $(TESTS) $(UTILS) a.out *.o 

