from matplotlib import pyplot as plt


def get_coords(line):
    return [-int(x) for x in line.split()]


def main():
    with open("res.txt", "r") as f:
        x = 1
        fig = plt.figure()
        ax = fig.add_subplot(111)

        lines = f.readlines()
        sizes = get_coords(lines[0])
        for i in range(sizes[0]+1, 0+1):
            ax.plot((0, len(lines)), (i, i), 'r')
        for i in range(sum(sizes)+1, sizes[0]+1):
            ax.plot((0, len(lines)), (i, i), 'b')

        for line in lines[1:-2]:
            coords = get_coords(line)
            ax.plot((x, x), coords, '-ok')
            x += 1
        
        ax.set_axisbelow(True)
        ax.minorticks_on()
        ax.grid(which='minor', linestyle=':', linewidth='0.5', color='black')
        ax.grid()
        plt.show()  # show all figures


if __name__ == '__main__':
    main()

