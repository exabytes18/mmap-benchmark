#!/usr/bin/python

import locale
import sys
import matplotlib.pyplot as plt

access_types = {
    'sequential_read': {
        'desc': 'Sequential Reads',
        'verb': 'read',
        'short_desc': 'reads',
    },
    'sequential_write': {
        'desc': 'Sequential Writes',
        'verb': 'written',
        'short_desc': 'writes',
    },
    'random_read': {
        'desc': 'Random Reads',
        'verb': 'read',
        'short_desc': 'reads',
    },
    'random_write': {
        'desc': 'Random Writes',
        'verb': 'written',
        'short_desc': 'writes',
    },
}


# from http://stackoverflow.com/a/1094933
def sizeof_fmt(num):
    for x in ['bytes','KB','MB','GB']:
        if num < 1024.0 and num > -1024.0:
            return "%3.1f%s" % (num, x)
        num /= 1024.0
    return "%3.1f%s" % (num, 'TB')


def cumulative(arr):
    total = 0
    cumulative_array = []
    for e in arr:
        total += e
        cumulative_array.append(total)
    return cumulative_array


def test_desc_tuple_to_title(test_desc_tuple):
    size = sizeof_fmt(test_desc_tuple[0])
    locale.setlocale(locale.LC_ALL, 'en_US')
    page_accesses = locale.format("%d", test_desc_tuple[1], grouping=True)
    desc = access_types[test_desc_tuple[2]]['desc']
    short_desc = access_types[test_desc_tuple[2]]['short_desc']
    return "%s (%s file; %s 4KB %s)" % (desc, size, page_accesses, short_desc)


def generate_graph(test_desc_tuple, data_by_flags):

    verb = access_types[test_desc_tuple[2]]['verb']

    first = True
    x = []
    megabytes_per_sec_by_flags = {}
    pages_per_sec_by_flags = {}

    for flags, data in data_by_flags.iteritems():
        bytes_accessed = map(lambda x: x[0], data)
        megabytes_per_sec = map(lambda x: x[1]/1024/1024, data)
        pages_per_sec = map(lambda x: x[2], data)
        if first:
            x = bytes_accessed
            first = False
        elif x != bytes_accessed:
            print "Data are not aligned!!"
            sys.exit(1)

        megabytes_per_sec_by_flags[flags] = megabytes_per_sec
        pages_per_sec_by_flags[flags] = pages_per_sec

    cumulative_bytes_accessed = cumulative(x)
    cumulative_gigabytes_accessed = map(lambda x: float(x)/1024**3, cumulative_bytes_accessed)

    xmax = max(cumulative_gigabytes_accessed)
    ymax = max(map(lambda x: max(x), megabytes_per_sec_by_flags.values()))

    fig, ax1 = plt.subplots(figsize=(16,10))
    plt.title(test_desc_tuple_to_title(test_desc_tuple))
    plt.axis([0, xmax, 0, ymax])
    ax1.set_xlabel("Gigabytes %s" % verb)
    ax1.set_ylabel("Throughput (MBps)")
    ax1.grid()

    for flag, data in megabytes_per_sec_by_flags.iteritems():
        ax1.plot(cumulative_gigabytes_accessed, data, label=flag)

    ax1.legend(loc=1)

    #ax2 = ax1.twinx()
    #s2 = np.sin(2*np.pi*t)
    #ax2.plot(t, s2, 'r.')
    #ax2.set_ylabel('sin', color='r')
    #for tl in ax2.get_yticklabels():
    #    tl.set_color('r')
    plt.tight_layout()
    plt.show()


def generate_graphs(data_by_test):
    for test_desc_tuple, data_by_flags in data_by_test.iteritems():
        generate_graph(test_desc_tuple, data_by_flags)


def read_tuples(filename):
    with open(filename) as f:
        tuples = []
        for line in f.readlines():
            values = line.split(',')
            tuples.append((
                int(values[0]),     # test.file_size
                int(values[1]),     # test.page_accesses
                values[2].strip(),  # flags
                values[3].strip(),  # access type
                int(values[4]),     # pages accessed during measurement
                int(values[5]),     # bytes accessed during measurement
                float(values[6]),   # time in seconds for measurement
                float(values[7]),   # bytes/second for measurement
                float(values[8])    # pages/second for measurement
            ))
        return tuples


def split_by_test(tuples):
    data_by_test = {}
    for t in tuples:
        test_file_size = t[0]
        test_page_accesses = t[1]
        flags = t[2]
        access_type = t[3]
        bytes_accessed = t[5]
        bytes_per_sec = t[7]
        pages_per_sec = t[8]
        test_desc_tuple = (test_file_size, test_page_accesses, access_type)
        data_by_test.setdefault(test_desc_tuple, {}) \
                    .setdefault(flags, []) \
                    .append((
                        bytes_accessed,
                        bytes_per_sec,
                        pages_per_sec
                    ))
    return data_by_test


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print "usage: %s file.csv [file2.csv ...]" % sys.argv[0]
        sys.exit(1)

    for filename in sys.argv[1:]:
        tuples = read_tuples(filename)
        data_by_test = split_by_test(tuples)
        generate_graphs(data_by_test)

    sys.exit(0)
