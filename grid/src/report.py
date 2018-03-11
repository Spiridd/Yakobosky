from matplotlib import pyplot as plt
from math import log
import os


def get_filenames(dir_name):
    filepath = os.path.abspath(__file__)
    cur_dir = os.path.dirname(filepath)
    dir_name_abs = os.path.join(cur_dir, dir_name)
    return [f for f in os.listdir(dir_name_abs)]


def read_data_from_files(filenames, files_dir):
    data = []
    for filename in filenames:
        with open(os.path.join(files_dir, filename), 'r') as f:
            record = {}
            record['time'] = float(f.readline())
            point1 = filename.index('.')
            record['mode'] = filename[:point1]
            point2 = filename.index('.', point1+1)
            record['n_proc'] = int(filename[point1+1:point2])
            point3 = filename.index('.', point2+1)
            record['size'] = int(filename[point2+1:point3])
            data.append(record)
    return data


def plot_time(data):
    # add effectiveness, speedup
    sizes = set([d['size'] for d in data])
    for size in sizes:
        some_data = [(d['time'], d['n_proc']) for d in data if d['size']==size]
        some_data = sorted(some_data, key=lambda x: x[1])
        keys = [x[1] for x in some_data]
        values = [x[0] for x in some_data]
        print(size, values)
        plt.semilogx(keys, values, ':o', label=str(size), basex=2)
    plt.title('time')
    plt.xlabel('number of processes')
    plt.ylabel('time, sec')
    plt.legend()
    plt.grid()
    plt.show()


def plot_speedup(data):
    sizes = set([d['size'] for d in data])
    for size in sizes:
        some_data = [(d['time'], d['n_proc']) for d in data if d['size']==size]
        some_data = sorted(some_data, key=lambda x: x[1])
        one_proc_time = some_data[0][0]
        keys = [x[1] for x in some_data]
        values = [one_proc_time/x[0] for x in some_data]
        plt.semilogx(keys, values, ':o', label=str(size), basex=2)
    plt.title('speedup')
    plt.xlabel('number of processes')
    plt.ylabel('speedup')
    plt.legend()
    plt.grid()
    plt.show()


def plot_effect(data):
    sizes = set([d['size'] for d in data])
    for size in sizes:
        some_data = [(d['time'], d['n_proc']) for d in data if d['size']==size]
        some_data = sorted(some_data, key=lambda x: x[1])
        one_proc_time = some_data[0][0]
        keys = [x[1] for x in some_data]
        values = [one_proc_time/x[0]/x[1]*100 for x in some_data]
        plt.semilogx(keys, values, ':o', label=str(size), basex=2)
    plt.title('effectiveness')
    plt.xlabel('number of processes')
    plt.ylabel('effectiveness')
    plt.legend()
    plt.grid()
    plt.show()


def plot_one_proc(data):
    some_data = [(d['time'], d['size']) for d in data if d['n_proc']==1]
    some_data = sorted(some_data, key=lambda x: x[1])
    keys = [x[1]*x[1] for x in some_data]
    values = [1e9*x[0]/(2*x[1]*x[1]*log(x[1], 2)) for x in some_data]
    [print('{} : {}'.format(k, v)) for k, v in zip(keys, values)]
    plt.semilogx(keys, values, ':o', basex=10)
    plt.xlabel('grid size')
    plt.ylabel('K=T(n)/nlogn')
    plt.grid()
    plt.show()


def main():
    files_dir = '../out'
    filenames = get_filenames(files_dir)
    data = read_data_from_files(filenames, files_dir)
    # provide more data (copy from BG/P)
    # plot_one_proc(data)
    plot_time(data)
    plot_speedup(data)
    plot_effect(data)


if __name__ == '__main__':
    main()

