CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude
SRC = src/main.c src/subject.c src/storage.c src/utils.c
OUT = attendance

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) -lm

run: $(OUT)
	./$(OUT)

clean:
	rm -f $(OUT)
