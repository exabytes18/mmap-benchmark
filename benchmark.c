#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define MEASUREMENT_FREQUENCY_IN_PAGE_ACCESSES (8192)

// You MUST attach ephemeral storage at creation time.
// use amazon linux (hvm) (ami-e9a18d80)

// Notes:
//   - Looks like scheduler=noop and rotational=0, by default with no alternatives to test
//   - "mount /something /somewhere"
//   - install xfs: "yum install xfsprogs"
//   - format and mount drive: mkfs.xfs /dev/xvdb; mkdir /mnt/disk1; mount /dev/xvdb /mnt/disk1

// Other considerations:
//   - try another AMI?
//   - try c3.large, i2.xlarge, i2.8xlarge
//   - try EXT4 and XFS
//   - single ephemeral, RAID ephemeral, EBS, EBS-Optimized
//   - HDD vs SSD
//   - MAP_HUGETLB, MAP_POPULATE, ... ?

// questions:
//   - what happens with the mmap when there are IO errors?

typedef struct {
    int flags;
    char *desc;
} file_flags_t;

typedef struct {
    size_t file_size;
    size_t page_accesses;
} test_size_t;

typedef struct {
    int (*function)(test_size_t test_size, file_flags_t file_flags, void *mem, size_t length, char *desc);
    char *desc;
} test_function_t;


// From http://stackoverflow.com/a/2999130
static int rand_lim(int limit) {
    int divisor = RAND_MAX/(limit+1);
    int retval;

    do {
        retval = rand() / divisor;
    } while(retval > limit);

    return retval;
}

static void **random_page_addresses(void *mem, size_t length) {
    long page_size = sysconf(_SC_PAGE_SIZE);

    size_t num_pages = length / page_size;
    void **random_pages = malloc(num_pages * sizeof(random_pages[0]));
    for(size_t i = 0; i < num_pages; i++) {
        random_pages[i] = mem + i * page_size;
    }

    for(size_t i = 0; i < num_pages; i++) {
        size_t r = i + rand_lim(num_pages - i - 1);
        if(i != r) {
            void *t = random_pages[i];
            random_pages[i] = random_pages[r];
            random_pages[r] = t;
        }
    }

    return random_pages;
}


static void log_measurement(test_size_t test_size, file_flags_t file_flags, char *desc, struct timeval *start, struct timeval *end, size_t bytes, double percent_complete) {
    time_t elapsed_time_us = end->tv_usec - start->tv_usec;
    elapsed_time_us += 1000000l * (end->tv_sec - start->tv_sec);

    // Use printf here only; all other output should go to stderr.
    double time_in_seconds = elapsed_time_us / 1e6;
    double bytes_per_second = bytes / time_in_seconds;
    double pages_per_second = MEASUREMENT_FREQUENCY_IN_PAGE_ACCESSES / time_in_seconds;
    printf("%ld,%ld,%s,%s,%d,%ld,%lf,%lf,%lf\n", test_size.file_size, test_size.page_accesses, file_flags.desc, desc, MEASUREMENT_FREQUENCY_IN_PAGE_ACCESSES, bytes, time_in_seconds, bytes_per_second, pages_per_second);

    // Dump some more human friendly metrics to stderr
    fprintf(stderr, "  %5.1lf complete (%9.3lf MBps, %10.1lf pages/sec)\n", percent_complete, bytes_per_second / 1024 / 1024, pages_per_second);
}


static int sequential_read(test_size_t test_size, file_flags_t file_flags, void *mem, size_t length, char *desc) {
    long page_size = sysconf(_SC_PAGE_SIZE);
    char buf[page_size];

    struct timeval start, end;
    size_t next_log = MEASUREMENT_FREQUENCY_IN_PAGE_ACCESSES;
    size_t page_accesses = test_size.page_accesses;
    size_t bytes_copied = 0;
    void *current_page = mem;

    gettimeofday(&start, NULL);
    for(size_t i = 0; i < page_accesses; i++) {
        memcpy(buf, current_page, page_size);
        bytes_copied += page_size;

        current_page += page_size;
        if(current_page >= mem + length) {
            // Loop back to the beginning
            current_page = mem;
        }

        // log performance periodically
        if(i+1 == next_log) {
            gettimeofday(&end, NULL);

            double percent_complete = ((double)i / page_accesses) * 100;
            log_measurement(test_size, file_flags, desc, &start, &end, bytes_copied, percent_complete);
            bytes_copied = 0;
            next_log += MEASUREMENT_FREQUENCY_IN_PAGE_ACCESSES;

            gettimeofday(&start, NULL);
        }
    }
    gettimeofday(&end, NULL);

    return 1;
}


static int sequential_write(test_size_t test_size, file_flags_t file_flags, void *mem, size_t length, char *desc) {
    long page_size = sysconf(_SC_PAGE_SIZE);
    char buf[page_size];

    struct timeval start, end;
    size_t next_log = MEASUREMENT_FREQUENCY_IN_PAGE_ACCESSES;
    size_t page_accesses = test_size.page_accesses;
    size_t bytes_copied = 0;
    void *current_page = mem;

    // initialize our buffer;
    for(size_t i = 0; i < sizeof(buf); i++) {
        buf[i] = (char)i;
    }

    gettimeofday(&start, NULL);
    for(size_t i = 0; i < page_accesses; i++) {
        memcpy(current_page, buf, page_size);
        bytes_copied += page_size;

        current_page += page_size;
        if(current_page >= mem + length) {
            // Loop back to the beginning
            current_page = mem;
        }

        // log performance periodically
        if(i+1 == next_log) {
            gettimeofday(&end, NULL);

            double percent_complete = ((double)i / page_accesses) * 100;
            log_measurement(test_size, file_flags, desc, &start, &end, bytes_copied, percent_complete);
            bytes_copied = 0;
            next_log += MEASUREMENT_FREQUENCY_IN_PAGE_ACCESSES;

            gettimeofday(&start, NULL);
        }
    }
    gettimeofday(&end, NULL);

    return 1;
}


static int random_read(test_size_t test_size, file_flags_t file_flags, void *mem, size_t length, char *desc) {
    long page_size = sysconf(_SC_PAGE_SIZE);
    char buf[page_size];

    size_t num_pages = length / page_size;
    void **random_pages = random_page_addresses(mem, length);
    if(random_pages == NULL) {
        fprintf(stderr, "uh oh, no more memory?\n");
        return 0;
    }

    struct timeval start, end;
    size_t next_log = MEASUREMENT_FREQUENCY_IN_PAGE_ACCESSES;
    size_t page_accesses = test_size.page_accesses;
    size_t bytes_copied = 0;
    size_t current_page_index = 0;

    gettimeofday(&start, NULL);
    for(size_t i = 0; i < page_accesses; i++) {
        memcpy(buf, random_pages[current_page_index], page_size);
        bytes_copied += page_size;

        current_page_index++;
        if(current_page_index >= num_pages) {
            // Loop back to the beginning
            current_page_index = 0;
        }

        // log performance periodically
        if(i+1 == next_log) {
            gettimeofday(&end, NULL);

            double percent_complete = ((double)i / page_accesses) * 100;
            log_measurement(test_size, file_flags, desc, &start, &end, bytes_copied, percent_complete);
            bytes_copied = 0;
            next_log += MEASUREMENT_FREQUENCY_IN_PAGE_ACCESSES;

            gettimeofday(&start, NULL);
        }
    }
    gettimeofday(&end, NULL);

    free(random_pages);
    return 1;
}


static int random_write(test_size_t test_size, file_flags_t file_flags, void *mem, size_t length, char *desc) {
    long page_size = sysconf(_SC_PAGE_SIZE);
    char buf[page_size];

    size_t num_pages = length / page_size;
    void **random_pages = random_page_addresses(mem, length);
    if(random_pages == NULL) {
        fprintf(stderr, "uh oh, no more memory?\n");
        return 0;
    }

    struct timeval start, end;
    size_t next_log = MEASUREMENT_FREQUENCY_IN_PAGE_ACCESSES;
    size_t page_accesses = test_size.page_accesses;
    size_t bytes_copied = 0;
    size_t current_page_index = 0;

    gettimeofday(&start, NULL);
    for(size_t i = 0; i < page_accesses; i++) {
        memcpy(random_pages[current_page_index], buf, page_size);
        bytes_copied += page_size;

        current_page_index++;
        if(current_page_index >= num_pages) {
            // Loop back to the beginning
            current_page_index = 0;
        }

        // log performance periodically
        if(i+1 == next_log) {
            gettimeofday(&end, NULL);

            double percent_complete = ((double)i / page_accesses) * 100;
            log_measurement(test_size, file_flags, desc, &start, &end, bytes_copied, percent_complete);
            bytes_copied = 0;
            next_log += MEASUREMENT_FREQUENCY_IN_PAGE_ACCESSES;

            gettimeofday(&start, NULL);
        }
    }
    gettimeofday(&end, NULL);

    free(random_pages);
    return 1;
}


static int create_uncached_initialized_file(const char *path, int additional_flags, size_t length) {
    int flags = O_WRONLY | O_CREAT | O_TRUNC;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

#ifdef __APPLE__
    int fd = open(path, flags, mode);
    if(fd == -1) {
        perror("problem opening file");
        return -1;
    }

    if(fcntl(fd, F_NOCACHE, 1) == -1) {
        perror("problem setting fd as F_NOCACHE");
        close(fd);
        return -1;
    }
#else
    int fd = open(path, flags | O_DIRECT, mode);
    if(fd == -1) {
        perror("problem opening file");
        return -1;
    }
#endif

    long page_size = sysconf(_SC_PAGE_SIZE);
    size_t buf_size = length >= 1024*1024 ? 1024*1024 : page_size;
    char buf[buf_size];
    for(size_t i = 0; i < sizeof(buf); i++) {
        buf[i] = (char)i;
    }

    size_t bytes_remaining = length;
    while(bytes_remaining > 0) {
        size_t bytes_to_write = bytes_remaining > sizeof(buf) ? sizeof(buf) : bytes_remaining;
        ssize_t bytes_written = write(fd, buf, bytes_to_write);
        if(bytes_written == -1) {
            perror("problem writing bytes to file");
            close(fd);
            return -1;
        }
        bytes_remaining -= bytes_written;
    }

    close(fd);

    fd = open(path, O_RDWR | additional_flags, mode);
    if(fd == -1) {
        perror("problem opening file");
        return -1;
    }

    return fd;
}


static int benchmark1(const char *file, const test_size_t test_size, const file_flags_t file_flags) {
    test_function_t test_functions[] = {
        {sequential_read, "sequential_read"},
        {sequential_write, "sequential_write"},
        {random_read, "random_read"},
        {random_write, "random_write"},
    };

    for(int j = 0; j < sizeof(test_functions)/sizeof(test_functions[0]); j++) {
        test_function_t test_function = test_functions[j];
        fprintf(stderr, "Running benchmarking: file_size=%ld, page_accesses=%ld, file_flags=%s, test=%s\n", test_size.file_size, test_size.page_accesses, file_flags.desc, test_function.desc);

        int fd = create_uncached_initialized_file(file, file_flags.flags, test_size.file_size);
        if(fd == -1) {
            return 0;
        }

        void *mem = mmap(NULL, test_size.file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if(mem == MAP_FAILED) {
            perror("problem mmapping file");
            close(fd);
            return 0;
        }

        if(!test_function.function(test_size, file_flags, mem, test_size.file_size, test_function.desc)) {
            munmap(mem, test_size.file_size);
            close(fd);
            return 0;
        }

        munmap(mem, test_size.file_size);
        close(fd);
    }

    return 1;
}


static int benchmark(const char *file, const test_size_t test_size) {
    file_flags_t flags[] = {
        {0, "NONE"},
        {O_SYNC, "O_SYNC"},
        {O_DSYNC, "O_DSYNC"},
#ifdef __linux
        // O_DIRECT is only available on linux
        {O_DIRECT, "O_DIRECT"},
        {O_DIRECT | O_SYNC, "O_DIRECT | O_SYNC"},
        {O_DIRECT | O_DSYNC, "O_DIRECT | O_DSYNC"},
#endif
    };
    for(int i = 0; i < sizeof(flags)/sizeof(flags[0]); i++) {
        if(!benchmark1(file, test_size, flags[i])) {
            return 0;
        }
    }

    return 1;
}


int parse_multiplier(const char *arg, size_t *result) {
    const char *str = arg;
    while(str != '\0' && isspace(*str)) {
        str++;
    }

    if(strcasecmp(str, "") == 0 || strcasecmp(str, "b") == 0) {
        *result = 1ULL;
        return 1;
    } else if(strcasecmp(str, "k") == 0 || strcasecmp(str, "kb") == 0) {
        *result = 1ULL << 10;
        return 1;
    } else if(strcasecmp(str, "m") == 0 || strcasecmp(str, "mb") == 0) {
        *result = 1ULL << 20;
        return 1;
    } else if(strcasecmp(str, "g") == 0 || strcasecmp(str, "gb") == 0) {
        *result = 1ULL << 30;
        return 1;
    } else if(strcasecmp(str, "t") == 0 || strcasecmp(str, "tb") == 0) {
        *result = 1ULL << 40;
        return 1;
    } else if(strcasecmp(str, "p") == 0 || strcasecmp(str, "pb") == 0) {
        *result = 1ULL << 40;
        return 1;
    } else if(strcasecmp(str, "e") == 0 || strcasecmp(str, "eb") == 0) {
        *result = 1ULL << 50;
        return 1;
    } else if(strcasecmp(str, "z") == 0 || strcasecmp(str, "zb") == 0) {
        fprintf(stderr, "zettabytes, really?\n");
        return 0;
    } else if(strcasecmp(str, "y") == 0 || strcasecmp(str, "yb") == 0) {
        fprintf(stderr, "yottabytes, impossible.\n");
        return 0;
    } else {
        fprintf(stderr, "unknown unit\n");
        return 0;
    }
}


int parse_arg(const char *arg, test_size_t *result) {
    long page_size = sysconf(_SC_PAGE_SIZE);

    char *comma = strchr(arg, ',');
    if(comma == NULL) {
        fprintf(stderr, "problem parsing arg; please specify both file_size and page_accesses\n");
        return 0;
    }

    size_t str1_len = comma - arg;
    char str1[str1_len + 1];
    strncpy(str1, arg, str1_len);
    str1[str1_len] = '\0';

    size_t str2_len = arg + strlen(arg) - comma - 1;
    char str2[str2_len + 1];
    strncpy(str2, comma + 1, str2_len);
    str2[str2_len] = '\0';

    char *ptr = NULL;

    errno = 0;
    ssize_t first_num = strtoll(str1, &ptr, 10);
    if(errno != 0 || str1 == ptr) {
        fprintf(stderr, "problem parsing string: %s\n", str1);
        return 0;
    }

    size_t multiplier;
    if(!parse_multiplier(ptr, &multiplier)) {
        fprintf(stderr, "problem parsing string: %s\n", ptr);
        return 0;
    }

    ssize_t second_num = strtoll(str2, &ptr, 10);
    if(errno != 0 || str2 == ptr) {
        fprintf(stderr, "problem parsing string: %s\n", str2);
        return 0;
    }

    // We /could/ overflow here, but it isn't worth checking.
    result->file_size = first_num * multiplier;
    if(result->file_size <= 0 || result->file_size % page_size != 0) {
        fprintf(stderr, "Invalid file_size; value must be a positive multiple of page size (%ld)\n", page_size);
        return 0;
    }

    result->page_accesses = second_num;
    if(result->page_accesses <= 0 || result->page_accesses % MEASUREMENT_FREQUENCY_IN_PAGE_ACCESSES != 0) {
        fprintf(stderr, "Invalid page_accesses; value must be a positive multiple of MEASUREMENT_FREQUENCY_IN_PAGE_ACCESSES (%d)\n", MEASUREMENT_FREQUENCY_IN_PAGE_ACCESSES);
        return 0;
    }

    return 1;
}


int main(int argc, const char **argv) {
    if(argc <= 1) {
        fprintf(stderr, "usage: ./benchmark FILE_SIZE,PAGE_ACCESSES ...\n");
        return EXIT_FAILURE;
    }

    // I'm usually against stack allocated structs as well as not declaring
    // variables at the beginning of functions, but I don't want to waste time
    // over-engineering this benchmark.
    test_size_t test_sizes[argc - 1];
    for(int i = 1 /* Ignore first */; i < argc; i++) {
        // [] has higher precedence than &
        if(!parse_arg(argv[i], &test_sizes[i-1])) {
            return EXIT_FAILURE;
        }
    }

    // print out the page size, for reference
    long page_size = sysconf(_SC_PAGE_SIZE);
    fprintf(stderr, "page_size: %ld\n", page_size);
    fprintf(stderr, "\n");

    // Run our benchmarks
    fprintf(stderr, "Running benchmarks...\n");
    for(int i = 0; i < sizeof(test_sizes)/sizeof(test_sizes[0]); i++) {
        if(!benchmark("benchmark.bin", test_sizes[i])) {
            return EXIT_FAILURE;
        }
    }


    fprintf(stderr, "Done!\n");
    return EXIT_SUCCESS;
}
