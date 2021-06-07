SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = output

EXE := $(BIN_DIR)/dpf
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CC = g++
CFLAGS = -Wall -g
CPPFLAGS = -I/opt/local/include -Iinclude -MMD -MP
LDFLAGS = -L/opt/local/lib -Llib
LDLIBS = -fopenmp -maes -lcryptopp -lcrypto


.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJ) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# libdpf: $(SRC_DIR)/libdpf/aes.h $(SRC_DIR)/libdpf/libdpf.h $(SRC_DIR)/libdpf/libdpf.c $(SRC_DIR)/libdpf/block.h $(SRC_DIR)/libdpf/block.c
# 	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(LDLIBS) -c $< -o $@

# dpf: $(SRC_DIR)/%.cpp $(SRC_DIR)/%.h
# 	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(LDLIBS) -c $< -o $@ libdpf.o

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR) # The @ disables the echoing of the command

-include $(OBJ:.o=.d)