CC=g++
CFLAGS=-Wall -Wextra
BIN_DIR=bin

SRCS=$(wildcard *.cpp)

BINS=$(patsubst %.cpp,$(BIN_DIR)/%,$(SRCS))

all: $(BINS)

$(BIN_DIR)/%: %.cpp | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $<

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

.PHONY: clean

clean:
	rm -rf $(BIN_DIR)

run: $(BINS)
	$(foreach bin,$(BINS),$(bin);)
