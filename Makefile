all: benchmark hugetlb

benchmark: benchmark.c
	gcc -O3 -std=c99 -Wall benchmark.c -o benchmark

run: benchmark
	./benchmark 64M,32768 128M,65536

run-16G: benchmark
	./benchmark 16G,67108864

hugetlb: hugetlb.c
	gcc -O3 -std=c99 -Wall hugetlb.c -o hugetlb
