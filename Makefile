LIBS = -lm
CFLAGS = -Wall
OBJS = tic ricker utils simulation velocity_model
UTILS = bfdiff d2f
PROGS = wave 
TESTS = test_tic test_ricker

SRC=$(wildcard *.c)

wave: $(OBJS) 
	gcc -c wave.c $(CGLAGS) 
	gcc -o wave wave.o ricker.o tic.o simulation.o velocity_model.o $(CGLAGS) $(LIBS)

all: $(OBJS) $(UTILS) $(PROGS)
	@echo Pick $(PROGS)

tests: $(TESTS)


##################################################
## LIBS
##################################################

objs: $(OBJS)

ricker:
	gcc -c ricker.c ${CFLAGS}

simulation:
	gcc -c simulation.c ${CFLAGS}

velocity_model:
	gcc -c velocity_model.c ${CFLAGS}

tic:
	gcc -c tic.c $(CFLAGS)

utils:
	gcc -c utils.c $(CFLAGS)

##################################################
## TESTS
##################################################

tests: objs $(TESTS)

test_tic: tic
	gcc -o test_tic test_tic.c tic.o $(LIBS) $(CFLAGS)

test_ricker: ricker
	gcc -o test_ricker test_ricker.c ricker.o $(CFLAGS) $(LIBS)


##################################################
## UTILS 
##################################################

bfdiff: utils
	gcc -o bfdiff bfdiff.c utils.o $(CFLAGS) 

d2f:
	gcc -o d2f d2f.c $(CFLAGS) 


##################################################
## Clean all 
##################################################
clean: 
	rm -f $(PROGS) $(TESTS) $(UTILS) a.out *.o 

