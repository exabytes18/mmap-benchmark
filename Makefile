all: benchmark hugetlb

benchmark: benchmark.c
	gcc -O3 -std=c99 -Wall benchmark.c -o benchmark

run: benchmark
	./benchmark 64M,32768 128M,65536

run-1G: benchmark
	./benchmark 1G,1048576

run-14G: benchmark
	./benchmark 14G,14680064

hugetlb: hugetlb.c
	gcc -O3 -std=c99 -Wall hugetlb.c -o hugetlb
