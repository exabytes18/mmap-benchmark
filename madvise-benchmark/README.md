# POSIX_MADV_NORMAL vs. POSIX_MADV_RANDOM vs. POSIX_MADV_SEQUENTIAL vs. POSIX_MADV_WILLNEED vs. POSIX_MADV_DONTNEED

## Hardware
c5d.2xlarge:
* 8 core CPU (Intel(R) Xeon(R) Platinum 8124M CPU @ 3.00GHz)
* 16gb RAM
* 200gb NVMe
* AMI:  amzn2-ami-hvm-2.0.20211001.1-x86_64-gp2
* Kernel: 4.14.246

## Results

### 1 GiB File

#### 1 thread, Random Writes (1 GiB file, 2 passes)
![1 thread, Random Writes (1 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/1073741824-2passes-1threads-random_writes.png)

#### 2 threads, Random Writes (1 GiB file, 2 passes)
![2 threads, Random Writes (1 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/1073741824-2passes-2threads-random_writes.png)

#### 4 threads, Random Writes (1 GiB file, 2 passes)
![4 threads, Random Writes (1 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/1073741824-2passes-4threads-random_writes.png)

#### 8 threads, Random Writes (1 GiB file, 2 passes)
![8 threads, Random Writes (1 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/1073741824-2passes-8threads-random_writes.png)

#### 16 threads, Random Writes (1 GiB file, 2 passes)
![16 threads, Random Writes (1 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/1073741824-2passes-16threads-random_writes.png)

#### 32 threads, Random Writes (1 GiB file, 2 passes)
![32 threads, Random Writes (1 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/1073741824-2passes-32threads-random_writes.png)

#### 64 threads, Random Writes (1 GiB file, 2 passes)
![64 threads, Random Writes (1 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/1073741824-2passes-64threads-random_writes.png)

#### POSIX_MADV_WILLNEED, Random Writes (1 GiB file, 2 passes)
![POSIX_MADV_WILLNEED, Random Writes (1 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/1073741824-2passes-posix_madv_willneed-random_writes.png)

#### POSIX_MADV_NORMAL, Random Writes (1 GiB file, 2 passes)
![POSIX_MADV_NORMAL, Random Writes (1 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/1073741824-2passes-posix_madv_normal-random_writes.png)

#### POSIX_MADV_SEQUENTIAL, Random Writes (1 GiB file, 2 passes)
![POSIX_MADV_SEQUENTIAL, Random Writes (1 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/1073741824-2passes-posix_madv_sequential-random_writes.png)

#### POSIX_MADV_DONTNEED, Random Writes (1 GiB file, 2 passes)
![POSIX_MADV_DONTNEED, Random Writes (1 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/1073741824-2passes-posix_madv_dontneed-random_writes.png)

#### POSIX_MADV_RANDOM, Random Writes (1 GiB file, 2 passes)
![POSIX_MADV_RANDOM, Random Writes (1 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/1073741824-2passes-posix_madv_random-random_writes.png)

#### 1 thread, Random Reads (1 GiB file, 2 passes)
![1 thread, Random Reads (1 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/1073741824-2passes-1threads-random_reads.png)

#### 2 threads, Random Reads (1 GiB file, 2 passes)
![2 threads, Random Reads (1 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/1073741824-2passes-2threads-random_reads.png)

#### 4 threads, Random Reads (1 GiB file, 2 passes)
![4 threads, Random Reads (1 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/1073741824-2passes-4threads-random_reads.png)

#### 8 threads, Random Reads (1 GiB file, 2 passes)
![8 threads, Random Reads (1 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/1073741824-2passes-8threads-random_reads.png)

#### 16 threads, Random Reads (1 GiB file, 2 passes)
![16 threads, Random Reads (1 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/1073741824-2passes-16threads-random_reads.png)

#### 32 threads, Random Reads (1 GiB file, 2 passes)
![32 threads, Random Reads (1 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/1073741824-2passes-32threads-random_reads.png)

#### 64 threads, Random Reads (1 GiB file, 2 passes)
![64 threads, Random Reads (1 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/1073741824-2passes-64threads-random_reads.png)

#### POSIX_MADV_WILLNEED, Random Reads (1 GiB file, 2 passes)
![POSIX_MADV_WILLNEED, Random Reads (1 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/1073741824-2passes-posix_madv_willneed-random_reads.png)

#### POSIX_MADV_NORMAL, Random Reads (1 GiB file, 2 passes)
![POSIX_MADV_NORMAL, Random Reads (1 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/1073741824-2passes-posix_madv_normal-random_reads.png)

#### POSIX_MADV_SEQUENTIAL, Random Reads (1 GiB file, 2 passes)
![POSIX_MADV_SEQUENTIAL, Random Reads (1 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/1073741824-2passes-posix_madv_sequential-random_reads.png)

#### POSIX_MADV_DONTNEED, Random Reads (1 GiB file, 2 passes)
![POSIX_MADV_DONTNEED, Random Reads (1 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/1073741824-2passes-posix_madv_dontneed-random_reads.png)

#### POSIX_MADV_RANDOM, Random Reads (1 GiB file, 2 passes)
![POSIX_MADV_RANDOM, Random Reads (1 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/1073741824-2passes-posix_madv_random-random_reads.png)

#### 1 thread, Sequential Writes (1 GiB file, 2 passes)
![1 thread, Sequential Writes (1 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/1073741824-2passes-1threads-seq_writes.png)

#### POSIX_MADV_WILLNEED, Sequential Writes (1 GiB file, 2 passes)
![POSIX_MADV_WILLNEED, Sequential Writes (1 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/1073741824-2passes-posix_madv_willneed-seq_writes.png)

#### POSIX_MADV_NORMAL, Sequential Writes (1 GiB file, 2 passes)
![POSIX_MADV_NORMAL, Sequential Writes (1 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/1073741824-2passes-posix_madv_normal-seq_writes.png)

#### POSIX_MADV_SEQUENTIAL, Sequential Writes (1 GiB file, 2 passes)
![POSIX_MADV_SEQUENTIAL, Sequential Writes (1 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/1073741824-2passes-posix_madv_sequential-seq_writes.png)

#### POSIX_MADV_DONTNEED, Sequential Writes (1 GiB file, 2 passes)
![POSIX_MADV_DONTNEED, Sequential Writes (1 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/1073741824-2passes-posix_madv_dontneed-seq_writes.png)

#### POSIX_MADV_RANDOM, Sequential Writes (1 GiB file, 2 passes)
![POSIX_MADV_RANDOM, Sequential Writes (1 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/1073741824-2passes-posix_madv_random-seq_writes.png)

#### 1 thread, Sequential Reads (1 GiB file, 2 passes)
![1 thread, Sequential Reads (1 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/1073741824-2passes-1threads-seq_reads.png)

#### POSIX_MADV_WILLNEED, Sequential Reads (1 GiB file, 2 passes)
![POSIX_MADV_WILLNEED, Sequential Reads (1 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/1073741824-2passes-posix_madv_willneed-seq_reads.png)

#### POSIX_MADV_NORMAL, Sequential Reads (1 GiB file, 2 passes)
![POSIX_MADV_NORMAL, Sequential Reads (1 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/1073741824-2passes-posix_madv_normal-seq_reads.png)

#### POSIX_MADV_SEQUENTIAL, Sequential Reads (1 GiB file, 2 passes)
![POSIX_MADV_SEQUENTIAL, Sequential Reads (1 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/1073741824-2passes-posix_madv_sequential-seq_reads.png)

#### POSIX_MADV_DONTNEED, Sequential Reads (1 GiB file, 2 passes)
![POSIX_MADV_DONTNEED, Sequential Reads (1 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/1073741824-2passes-posix_madv_dontneed-seq_reads.png)

#### POSIX_MADV_RANDOM, Sequential Reads (1 GiB file, 2 passes)
![POSIX_MADV_RANDOM, Sequential Reads (1 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/1073741824-2passes-posix_madv_random-seq_reads.png)

### 32 GiB File

#### 1 thread, Random Writes (32 GiB file, 2 passes)
![1 thread, Random Writes (32 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/34359738368-2passes-1threads-random_writes.png)

#### 2 threads, Random Writes (32 GiB file, 2 passes)
![2 threads, Random Writes (32 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/34359738368-2passes-2threads-random_writes.png)

#### 4 threads, Random Writes (32 GiB file, 2 passes)
![4 threads, Random Writes (32 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/34359738368-2passes-4threads-random_writes.png)

#### 8 threads, Random Writes (32 GiB file, 2 passes)
![8 threads, Random Writes (32 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/34359738368-2passes-8threads-random_writes.png)

#### 16 threads, Random Writes (32 GiB file, 2 passes)
![16 threads, Random Writes (32 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/34359738368-2passes-16threads-random_writes.png)

#### 32 threads, Random Writes (32 GiB file, 2 passes)
![32 threads, Random Writes (32 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/34359738368-2passes-32threads-random_writes.png)

#### 64 threads, Random Writes (32 GiB file, 2 passes)
![64 threads, Random Writes (32 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/34359738368-2passes-64threads-random_writes.png)

#### POSIX_MADV_WILLNEED, Random Writes (32 GiB file, 2 passes)
![POSIX_MADV_WILLNEED, Random Writes (32 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/34359738368-2passes-posix_madv_willneed-random_writes.png)

#### POSIX_MADV_NORMAL, Random Writes (32 GiB file, 2 passes)
![POSIX_MADV_NORMAL, Random Writes (32 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/34359738368-2passes-posix_madv_normal-random_writes.png)

#### POSIX_MADV_SEQUENTIAL, Random Writes (32 GiB file, 2 passes)
![POSIX_MADV_SEQUENTIAL, Random Writes (32 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/34359738368-2passes-posix_madv_sequential-random_writes.png)

#### POSIX_MADV_DONTNEED, Random Writes (32 GiB file, 2 passes)
![POSIX_MADV_DONTNEED, Random Writes (32 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/34359738368-2passes-posix_madv_dontneed-random_writes.png)

#### POSIX_MADV_RANDOM, Random Writes (32 GiB file, 2 passes)
![POSIX_MADV_RANDOM, Random Writes (32 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/34359738368-2passes-posix_madv_random-random_writes.png)

#### 1 thread, Random Reads (32 GiB file, 2 passes)
![1 thread, Random Reads (32 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/34359738368-2passes-1threads-random_reads.png)

#### 2 threads, Random Reads (32 GiB file, 2 passes)
![2 threads, Random Reads (32 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/34359738368-2passes-2threads-random_reads.png)

#### 4 threads, Random Reads (32 GiB file, 2 passes)
![4 threads, Random Reads (32 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/34359738368-2passes-4threads-random_reads.png)

#### 8 threads, Random Reads (32 GiB file, 2 passes)
![8 threads, Random Reads (32 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/34359738368-2passes-8threads-random_reads.png)

#### 16 threads, Random Reads (32 GiB file, 2 passes)
![16 threads, Random Reads (32 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/34359738368-2passes-16threads-random_reads.png)

#### 32 threads, Random Reads (32 GiB file, 2 passes)
![32 threads, Random Reads (32 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/34359738368-2passes-32threads-random_reads.png)

#### 64 threads, Random Reads (32 GiB file, 2 passes)
![64 threads, Random Reads (32 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/34359738368-2passes-64threads-random_reads.png)

#### POSIX_MADV_WILLNEED, Random Reads (32 GiB file, 2 passes)
![POSIX_MADV_WILLNEED, Random Reads (32 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/34359738368-2passes-posix_madv_willneed-random_reads.png)

#### POSIX_MADV_NORMAL, Random Reads (32 GiB file, 2 passes)
![POSIX_MADV_NORMAL, Random Reads (32 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/34359738368-2passes-posix_madv_normal-random_reads.png)

#### POSIX_MADV_SEQUENTIAL, Random Reads (32 GiB file, 2 passes)
![POSIX_MADV_SEQUENTIAL, Random Reads (32 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/34359738368-2passes-posix_madv_sequential-random_reads.png)

#### POSIX_MADV_DONTNEED, Random Reads (32 GiB file, 2 passes)
![POSIX_MADV_DONTNEED, Random Reads (32 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/34359738368-2passes-posix_madv_dontneed-random_reads.png)

#### POSIX_MADV_RANDOM, Random Reads (32 GiB file, 2 passes)
![POSIX_MADV_RANDOM, Random Reads (32 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/34359738368-2passes-posix_madv_random-random_reads.png)

#### 1 thread, Sequential Writes (32 GiB file, 2 passes)
![1 thread, Sequential Writes (32 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/34359738368-2passes-1threads-seq_writes.png)

#### POSIX_MADV_WILLNEED, Sequential Writes (32 GiB file, 2 passes)
![POSIX_MADV_WILLNEED, Sequential Writes (32 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/34359738368-2passes-posix_madv_willneed-seq_writes.png)

#### POSIX_MADV_NORMAL, Sequential Writes (32 GiB file, 2 passes)
![POSIX_MADV_NORMAL, Sequential Writes (32 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/34359738368-2passes-posix_madv_normal-seq_writes.png)

#### POSIX_MADV_SEQUENTIAL, Sequential Writes (32 GiB file, 2 passes)
![POSIX_MADV_SEQUENTIAL, Sequential Writes (32 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/34359738368-2passes-posix_madv_sequential-seq_writes.png)

#### POSIX_MADV_DONTNEED, Sequential Writes (32 GiB file, 2 passes)
![POSIX_MADV_DONTNEED, Sequential Writes (32 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/34359738368-2passes-posix_madv_dontneed-seq_writes.png)

#### POSIX_MADV_RANDOM, Sequential Writes (32 GiB file, 2 passes)
![POSIX_MADV_RANDOM, Sequential Writes (32 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/34359738368-2passes-posix_madv_random-seq_writes.png)

#### 1 thread, Sequential Reads (32 GiB file, 2 passes)
![1 thread, Sequential Reads (32 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/34359738368-2passes-1threads-seq_reads.png)

#### POSIX_MADV_WILLNEED, Sequential Reads (32 GiB file, 2 passes)
![POSIX_MADV_WILLNEED, Sequential Reads (32 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/34359738368-2passes-posix_madv_willneed-seq_reads.png)

#### POSIX_MADV_NORMAL, Sequential Reads (32 GiB file, 2 passes)
![POSIX_MADV_NORMAL, Sequential Reads (32 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/34359738368-2passes-posix_madv_normal-seq_reads.png)

#### POSIX_MADV_SEQUENTIAL, Sequential Reads (32 GiB file, 2 passes)
![POSIX_MADV_SEQUENTIAL, Sequential Reads (32 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/34359738368-2passes-posix_madv_sequential-seq_reads.png)

#### POSIX_MADV_DONTNEED, Sequential Reads (32 GiB file, 2 passes)
![POSIX_MADV_DONTNEED, Sequential Reads (32 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/34359738368-2passes-posix_madv_dontneed-seq_reads.png)

#### POSIX_MADV_RANDOM, Sequential Reads (32 GiB file, 2 passes)
![POSIX_MADV_RANDOM, Sequential Reads (32 GiB file, 2 passes)](http://static.laazy.com/mmap-benchmark/4/34359738368-2passes-posix_madv_random-seq_reads.png)
