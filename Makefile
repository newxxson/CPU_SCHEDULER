CC=gcc
CFLAGS=-Iconfig -Ischeduler
DEPS = config/config.h scheduler/scheduler.h
OBJ =  $(wildcard scheduler/*.c) $(wildcard config/*.c) run_evaluation.o


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

run_evaluation: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f $(OBJ) run_evaluation
