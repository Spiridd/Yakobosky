CC = gcc
N = 512
NUMBERS = 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25
build: gen
run: generate plot
generate: gen
	@./gen $(N) > res.txt
gen: main.o
	@$(CC) -o gen main.c
main.o: main.c
	@$(CC) -c -o main.o main.c
plot:
	@python3 plot.py
test: tester gen
	@for num in $(NUMBERS); \
	do \
		./gen $$num > res.txt; \
		./tester; \
	done
tester: tester.o
	@$(CC) -o tester tester.o
tester.o: tester.c
	@$(CC) -c -o tester.o tester.c
clean:
	-@rm *.o gen tester res.txt
