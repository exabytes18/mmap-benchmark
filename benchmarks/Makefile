CC:=gcc
OPTS:=-pedantic -Wall -Wextra -std=c99

all: sequential-access-benchmark random-access-benchmark

.PHONY: clean

sequential-access-benchmark: sequential-access-benchmark.o shared.o
	$(CC) $(OPTS) -o $@ $^

sequential-access-benchmark.o: sequential-access-benchmark.c
	$(CC) $(OPTS) -c $^

random-access-benchmark: random-access-benchmark.o shared.o bq.o
	$(CC) $(OPTS) -lpthread -o $@ $^

random-access-benchmark.o: random-access-benchmark.c
	$(CC) $(OPTS) -c $^

shared.o: shared.c
	$(CC) $(OPTS) -c $^

bq.o: bq.c
	$(CC) $(OPTS) -c $^

clean:
	rm -rf sequential-access-benchmark sequential-access-benchmark.o shared.o random-access-benchmark random-access-benchmark.o bq.o
