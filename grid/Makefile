CC = gcc
N = 1000
M = 1000
FILE = res.bin
build: generate serial
generate: generate.o
	@$(CC) -o generate generate.o
generate.o: generate.c
	@$(CC) -c -o generate.o generate.c
serial: serial.o
	@$(CC) -o serial serial.o
serial.o: serial.c
	@$(CC) -c -o serial.o serial.c
run: build
	@./generate $(N) $(M) $(FILE)
	@./serial $(FILE)
clean:
	-@rm *.o res.bin generate serial
