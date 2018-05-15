LIBS = -lm 
CFLAGS = -Wall -g -ggdb  -Wpedantic -Wextra  -O3
# -pg
OBJS = tic ricker utils simulation velocity_model globals wavefield help parse domain_comm parsempi
UTILS = bfdiff bddiff d2f d2a create_model
TESTS = test_tic test_ricker test_velocity_model test_sub_model test_domain_comm
CC = gcc
MPICC = mpicc

SRC=$(wildcard *.c)

all: $(OBJS) $(UTILS) $(PROGS) $(TESTS)

wave: $(OBJS) 
	$(CC) -c wave.c $(CFLAGS) 
	$(CC) -o wave wave.o help.o wavefield.o globals.o ricker.o tic.o simulation.o velocity_model.o parse.o $(CFLAGS) $(LIBS)

wavempi: $(OBJS) 
	$(MPICC) -c wavempi.c $(CFLAGS) 
	$(MPICC) -o wavempi wavempi.o help.o wavefield.o globals.o ricker.o tic.o simulation.o velocity_model.o parsempi.o domain_comm.o $(CFLAGS) $(LIBS)

tests: $(TESTS)


##################################################
## LIBS
##################################################

objs: $(OBJS)

parse:
	gcc -c parse.c $(CFLAGS)

parsempi:
	gcc -c parsempi.c $(CFLAGS)

help:
	gcc -c help.c $(CFLAGS)

globals:
	$(CC) -c globals.c $(CFLAGS)

ricker:
	gcc -c ricker.c $(CFLAGS)

simulation: 
	gcc -c simulation.c $(CFLAGS)

velocity_model: globals
	gcc -c velocity_model.c ${CFLAGS}

tic:
	gcc -c tic.c $(CFLAGS)

utils:
	gcc -c utils.c $(CFLAGS)

wavefield:
	gcc -c wavefield.c $(CFLAGS)

domain_comm:
	gcc -c domain_comm.c $(CFLAGS)


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

test_domain_comm: domain_comm
	$(CC) -o test_domain_comm test_domain_comm.c domain_comm.o $(CFLAGS) $(LIBS)

##################################################
## UTILS 
##################################################

create_model: velocity_model
	$(CC) -o create_model create_model.c velocity_model.o $(CFLAGS) $(LIBS)

create_source: ricker
	$(CC) -o create_source create_source.c ricker.o $(CFLAGS) $(LIBS)

get_dt: ricker velocity_model simulation tic
	$(CC) -o get_dt get_dt.c ricker.o velocity_model.o simulation.o tic.o $(CFLAGS) $(LIBS)

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

