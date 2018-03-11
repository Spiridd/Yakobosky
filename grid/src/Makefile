CC = gcc
PARCC = mpicc
PARRUN = mpirun
NPROC = 1
N = 1000
M = $(N)
INFILE = ../res/in.bin
OUTFILE = ../res/out.bin
STDOUT = > ../out/$(NPROC).$(N).out
build: generator serial parallel
generator: generator.o point.o
	@$(CC) -o generator generator.o point.o
generator.o: generator.c
	@$(CC) -c -o generator.o generator.c
point.o: point.c
	@$(PARCC) -c -o point.o point.c
serial: serial.o sort.o point.o
	@$(PARCC) -o serial serial.o sort.o point.o
serial.o: serial.c
	@$(PARCC) -c -o serial.o serial.c
sort.o: sort.c
	@$(PARCC) -c -o sort.o sort.c
generate: generator
	@./generator $(N) $(M) $(INFILE)
sort_seq: serial
	@$(PARRUN) -n $(NPROC) ./serial $(INFILE) $(OUTFILE) $(STDOUT)
sort_par: parallel
	@$(PARRUN) -n $(NPROC) ./parallel $(INFILE) $(OUTFILE) $(STDOUT)
parallel: parallel.o sort.o point.o
	@$(PARCC) -o parallel parallel.o sort.o point.o
parallel.o: parallel.c
	@$(PARCC) -c -o parallel.o parallel.c
test: tester
	@./tester $(OUTFILE)
tester: tester.o sort.o point.o
	@$(CC) -o tester tester.o sort.o point.o
tester.o: tester.c
	@$(CC) -c -o tester.o tester.c
run: generate sort_seq
report:
	@python3 report.py
clean:
	-@rm *.o $(INFILE) $(OUTFILE) generator serial tester parallel
