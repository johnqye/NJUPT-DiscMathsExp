CFLAGS = -O2 -fno-strict-aliasing -Wall -Wextra -Werror
SRC = $(wildcard *.c)
BIN = $(SRC:.c=.elf)

%.elf: %.c
	$(CC) $(CFLAGS) $^ -o $@

all: $(BIN)
	cd Exp4; make vis -j16

clean:
	$(RM) $(BIN)
	cd Exp4; make clean
