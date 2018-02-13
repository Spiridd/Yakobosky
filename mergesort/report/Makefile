CC=gcc
IN = main.c
OUT = run
INCLUDE = -I/usr/local/include
LINK = /usr/local/lib/libpapi.a
CFLAGS=-std=c99 -O0
all:
	@date
	@$(CC) $(INCLUDE) $(CFLAGS) $(IN) $(LINK) -o $(OUT)
	@sudo ./$(OUT)
	@rm $(OUT)
	@date
	@sudo shutdown 0
