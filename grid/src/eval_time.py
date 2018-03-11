from math import log
from matplotlib import pyplot as plt


def amort(n):
    if n <= 1e4:
        return 180e-9
    elif n <= 1e6:
        return 185e-9
    else:
        return 190e-9


def time(n, p):
    # constants
    L = 0.1e-3  # latency, sec
    bandwidth = 425 * 1024 * 1024  # B/s
    nbytes = 12  # one object
    tau = nbytes * 1/bandwidth  # send one object, sec
    # variables
    piece = n/p
    K = amort(piece)
    log2p = log(p, 2)
    tacts = log2p*(log2p+1)/2
    return piece*(K*log(piece, 2)+(K+tau)*tacts) + L*tacts
    # return piece*(K*log(piece, 2)+(K+tau)*tacts)


def main():
    n = 8000*8000
    print('size = {}'.format(n))
    pmax = 128
    keys = [2**s for s in range(0, 1+int(log(pmax, 2)))]
    values = [time(n, p) for p in keys]
    print(values)
    # plot time
    plt.semilogx(keys, values, ':o', basex=2)
    plt.xlabel('number of processes')
    plt.ylabel('time, sec')
    plt.grid()
    plt.show()
    # plot speedup
    plt.figure()
    one_proc_time = time(n, 1)
    values = [one_proc_time/v for v in values]
    print(values)
    plt.semilogx(keys, values, ':o', basex=2)
    plt.xlabel('number of processes')
    plt.ylabel('speedup')
    plt.grid()
    plt.show()
    # plot speedup
    plt.figure()
    one_proc_time = time(n, 1)
    values = [v/k*100 for k, v in zip(keys, values)]
    print(values)
    plt.semilogx(keys, values, ':o', basex=2)
    plt.xlabel('number of processes')
    plt.ylabel('effectiveness')
    plt.grid()
    plt.show()


if __name__ == '__main__':
    main()

