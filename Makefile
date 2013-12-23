all: benchmark hugetlb

benchmark: benchmark.c
	gcc -O3 -std=c99 -Wall benchmark.c -o benchmark

run: benchmark
	./benchmark 64M,32768 128M,65536

run-huge: benchmark
	./benchmark 64M,4194304 256M,4194304 1G,4194304 4G,4194304 8G,67108864 16G,67108864 32G,67108864 64G,67108864 128G,268435456 256G,268435456 512G,268435456

hugetlb: hugetlb.c
	gcc -O3 -std=c99 -Wall hugetlb.c -o hugetlb
