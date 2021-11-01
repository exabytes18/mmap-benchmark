# NONE vs. O_SYNC vs. O_DSYNC vs. O_DIRECT vs. O_DIRECT | O_SYNC vs. O_DIRECT | O_DSYNC

## Hardware
c5d.2xlarge:
* 8 core CPU (Intel(R) Xeon(R) Platinum 8124M CPU @ 3.00GHz)
* 16gb RAM
* 200gb NVMe
* AMI:  amzn2-ami-hvm-2.0.20211001.1-x86_64-gp2
* Kernel: 4.14.246

## Results

### 1 GiB File

#### 1 thread, 1 SSD (ext4), sequential reads (1 GiB file, 16 GiB RAM)
![1073741824-c5d.2xlarge-seq-reads](http://static.laazy.com/mmap-benchmark/3/1073741824-c5d.2xlarge-seq-reads.png)

#### 1 thread, 1 SSD (ext4), sequential writes (1 GiB file, 16 GiB RAM)
![1073741824-c5d.2xlarge-seq-writes](http://static.laazy.com/mmap-benchmark/3/1073741824-c5d.2xlarge-seq-writes.png)

#### 1 thread, 1 SSD (ext4), random reads (1 GiB file, 16 GiB RAM)
![1073741824-c5d.2xlarge-rand-reads](http://static.laazy.com/mmap-benchmark/3/1073741824-c5d.2xlarge-rand-reads.png)

#### 1 thread, 1 SSD (ext4), random writes (1 GiB file, 16 GiB RAM)
![1073741824-c5d.2xlarge-rand-writes](http://static.laazy.com/mmap-benchmark/3/1073741824-c5d.2xlarge-rand-writes.png)

### 32 GiB File

#### 1 thread, 1 SSD (ext4), sequential reads (32 GiB file, 16 GiB RAM)
![34359738368-c5d.2xlarge-seq-reads](http://static.laazy.com/mmap-benchmark/3/34359738368-c5d.2xlarge-seq-reads.png)

#### 1 thread, 1 SSD (ext4), sequential writes (32 GiB file, 16 GiB RAM)
![34359738368-c5d.2xlarge-seq-writes](http://static.laazy.com/mmap-benchmark/3/34359738368-c5d.2xlarge-seq-writes.png)

#### 1 thread, 1 SSD (ext4), random reads (32 GiB file, 16 GiB RAM)
![34359738368-c5d.2xlarge-rand-reads](http://static.laazy.com/mmap-benchmark/3/34359738368-c5d.2xlarge-rand-reads.png)

#### 1 thread, 1 SSD (ext4), random writes (32 GiB file, 16 GiB RAM)
![34359738368-c5d.2xlarge-rand-writes](http://static.laazy.com/mmap-benchmark/3/34359738368-c5d.2xlarge-rand-writes.png)
