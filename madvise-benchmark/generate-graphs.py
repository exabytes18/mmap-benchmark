#!/usr/bin/env python

import sys
import matplotlib.pyplot as plt
from collections import OrderedDict

access_types = {
    'seq_reads': {
        'desc': 'Sequential Reads',
        'verb': 'read',
    },
    'seq_writes': {
        'desc': 'Sequential Writes',
        'verb': 'written',
    },
    'random_reads': {
        'desc': 'Random Reads (4KB)',
        'verb': 'read',
    },
    'random_writes': {
        'desc': 'Random Writes (4KB)',
        'verb': 'written',
    },
}


preferred_advice_order = [
    "POSIX_MADV_NORMAL",
    "POSIX_MADV_RANDOM",
    "POSIX_MADV_SEQUENTIAL",
    "POSIX_MADV_WILLNEED",
    "POSIX_MADV_DONTNEED",
]


# from http://stackoverflow.com/a/1094933
def sizeof_fmt(num):
    for x in ['bytes','KB','MB','GB']:
        if num < 1024.0 and num > -1024.0:
            return "%3.1f%s" % (num, x)
        num /= 1024.0
    return "%3.1f%s" % (num, 'TB')


def generate_graph(title, x_label, y_label, x, y, file_name):
    xmax = max(x)
    ymax = max(map(lambda a: max(a), y.values()))

    fig, ax1 = plt.subplots(figsize=(16,10))
    plt.title(title)
    plt.axis([0, xmax, 0, ymax])
    ax1.set_xlabel(x_label)
    ax1.set_ylabel(y_label)
    ax1.grid()

    for label, series in y.iteritems():
        ax1.plot(x, series, label=label)

    ax1.legend(loc=1)
    plt.tight_layout()
    plt.savefig(file_name)
    plt.close()


def is_match(d, **criteria):
    for k, v in criteria.iteritems():
        if d[k] != v:
            return False
    return True


def uniques(data, param, **criteria):
    uniques = set()
    for d in data:
        if is_match(d, **criteria):
            uniques.add(d[param])
    return uniques


def to_series(data, open_dimension, **criteria):
    x = {}
    y = {}
    for d in data:
        if is_match(d, **criteria):
            x.setdefault(d[open_dimension], []).append(float(d['total_bytes_copied']) / 1024**3)
            y.setdefault(d[open_dimension], []).append(float(d['bytes_per_second']) / 1024**2)

    expected = None
    for dimension, series in x.iteritems():
        if expected is None:
            expected = series
        else:
            if series != expected:
                raise Exception("mismatched data!")

    return (expected, y)


def pluralize(m, n, o):
    if m == 1:
        return "%d %s" % (m, n)
    else:
        return "%d %s%s" % (m, n, o)


def advice_order(d):
    ordered_dict = OrderedDict()
    for o in preferred_advice_order:
        if o in d:
            ordered_dict[o.replace("|", " | ")] = d[o]
    for k, v in d.iteritems():
        if k not in preferred_advice_order:
            ordered_dict[k.replace("|", " | ")] = v
    return ordered_dict


def thread_order(d):
    s = {}
    for k in sorted(d.keys()):
        s[pluralize(k, "thread", "s")] = d[k]
    return s


def generate_graphs(data):
    for file_size in uniques(data, 'file_size'):
        for num_passes in uniques(data, 'num_passes', file_size=file_size):
            for access_type, access_type_desc in access_types.iteritems():
                for num_threads in uniques(data, 'num_threads', file_size=file_size, num_passes=num_passes, access_type=access_type):
                    x, y = to_series(data, 'advice', file_size=file_size, num_passes=num_passes, access_type=access_type, num_threads=num_threads)
                    title = "%s (%s file; %s, %s)" % (
                        access_type_desc['desc'],
                        sizeof_fmt(file_size),
                        pluralize(num_passes, "pass", "es"),
                        pluralize(num_threads, "thread", "s")
                    )
                    file_name = "%d-%dpasses-%dthreads-%s.png" % (file_size, num_passes, num_threads, access_type)
                    generate_graph(title, "Gigabytes %s" % access_type_desc['verb'], "Throughput (MBps)", x, advice_order(y), file_name)
                for advice in uniques(data, 'advice', file_size=file_size, num_passes=num_passes, access_type=access_type):
                    x, y = to_series(data, 'num_threads', file_size=file_size, num_passes=num_passes, access_type=access_type, advice=advice)
                    title = "%s (%s file; %s, %s)" % (
                        access_type_desc['desc'],
                        sizeof_fmt(file_size),
                        pluralize(num_passes, "pass", "es"),
                        advice
                    )
                    file_name = "%d-%dpasses-%s-%s.png" % (file_size, num_passes, advice.lower().replace("|", "_"), access_type)
                    generate_graph(title, "Gigabytes %s" % access_type_desc['verb'], "Throughput (MBps)", x, thread_order(y), file_name)


def read_data(filename):
    with open(filename) as f:
        data = []
        for line in f.readlines():
            values = line.split(',')
            data.append({
                'file_size': int(values[0]),
                'num_passes': int(values[1]),
                'num_threads': int(values[2]),
                'advice': values[3].strip(),
                'access_type': values[4].strip(),
                'total_bytes_copied': int(values[5]),
                'bytes_copied': int(values[6]),
                'time_in_seconds': float(values[7]),
                'bytes_per_second': float(values[8]),
            })
        return data


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print "usage: %s output.csv [output2.csv ...]" % sys.argv[0]
        sys.exit(1)

    for filename in sys.argv[1:]:
        data = read_data(filename)
        generate_graphs(data)

    sys.exit(0)
