from matplotlib import pyplot as plt
from math import log


def get_data(file1, file2):
    d = {'sizes': [], 'times': [], 'moves': [], 'comps': []}
    with open(file1, 'r') as times, open(file2, 'r') as counts:
        for times_line, counts_line in zip(times, counts):
            n = int(times_line.split()[0])
            d['sizes'].append(n)
            d['times'].append(float(times_line.split()[1]))
            d['moves'].append(int(counts_line.split()[1]))
            d['comps'].append(int(counts_line.split()[2]))
    return d


def plot_data(*args):
    for arg in args:
        sizes = arg[1]['sizes']
        moves = arg[1]['moves']
        y = [m / s for s, m in zip(sizes, moves)]
        plt.semilogx(sizes, y, '.', label=arg[0])
    plt.legend()
    plt.xlabel('size of array')
    plt.ylabel('number of moves/n')
    plt.grid()

    plt.figure()
    for arg in args:
        sizes = arg[1]['sizes']
        comps = arg[1]['comps']
        y = [m / s for s, m in zip(sizes, comps)]
        plt.semilogx(sizes, y, '.', label=arg[0])
    plt.legend()
    plt.xlabel('size of array')
    plt.ylabel('number of comparisons/n')
    plt.grid()

    plt.figure()
    for arg in args:
        sizes = arg[1]['sizes']
        times = arg[1]['times']
        y = [t / s for s, t in zip(sizes, times)]
        plt.semilogx(sizes, y, '.', label=arg[0])
    plt.legend()
    plt.xlabel('size of array')
    plt.ylabel('time/n, sec')
    plt.grid()

    plt.figure()
    for arg in args:
        sizes = arg[1]['sizes']
        times = arg[1]['times']
        y = [1e9*t/s/log(s)*log(2) for s, t in zip(sizes, times)]
        plt.semilogx(sizes, y, '.', label=arg[0])
    plt.legend()
    plt.xlabel('size of array')
    plt.ylabel('time constant')
    plt.grid()

    plt.show()


def plot_diff(d1, d2):
    plt.semilogx(d1['sizes'], [y-z for y, z in zip(d1['moves'], d2['moves'])], '.')
    plt.grid()
    plt.show()


def main():
    f1_name = 'res/time_mergesort.txt'
    f2_name = 'res/count_mergesort.txt'
    f3_name = 'res/time_plain_mergesort.txt'
    f4_name = 'res/count_plain_mergesort.txt'
    merge = get_data(f1_name, f2_name)
    plain = get_data(f3_name, f4_name)
    special = merge.copy()
    special['moves'] = [1.5*log(n)/log(2) for n in special['sizes']]
    plot_data(('merge', merge), ('plain', plain))
    # plot_diff(special, plain)


if __name__ == '__main__':
    main()

