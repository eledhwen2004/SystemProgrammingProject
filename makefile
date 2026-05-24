CC=gcc
CFLAGS=-Wall -Wextra -std=c11

SRC_DIR=src
LIB_DIR=lib
BIN_DIR=bin

OBJ=$(BIN_DIR)/main.o \
    $(BIN_DIR)/archive.o \
    $(BIN_DIR)/extract.o \
    $(BIN_DIR)/utils.o

TARGET=tarsau

all: $(BIN_DIR) $(TARGET)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

$(BIN_DIR)/main.o: $(SRC_DIR)/main.c \
                    $(LIB_DIR)/archive.h \
                    $(LIB_DIR)/extract.h \
                    $(LIB_DIR)/utils.h
	$(CC) $(CFLAGS) -I$(LIB_DIR) -c $(SRC_DIR)/main.c -o $(BIN_DIR)/main.o

$(BIN_DIR)/archive.o: $(SRC_DIR)/archive.c \
                       $(LIB_DIR)/archive.h \
                       $(LIB_DIR)/utils.h
	$(CC) $(CFLAGS) -I$(LIB_DIR) -c $(SRC_DIR)/archive.c -o $(BIN_DIR)/archive.o

$(BIN_DIR)/extract.o: $(SRC_DIR)/extract.c \
                       $(LIB_DIR)/extract.h \
                       $(LIB_DIR)/utils.h
	$(CC) $(CFLAGS) -I$(LIB_DIR) -c $(SRC_DIR)/extract.c -o $(BIN_DIR)/extract.o

$(BIN_DIR)/utils.o: $(SRC_DIR)/utils.c \
                     $(LIB_DIR)/utils.h
	$(CC) $(CFLAGS) -I$(LIB_DIR) -c $(SRC_DIR)/utils.c -o $(BIN_DIR)/utils.o

clean:
	rm -rf $(BIN_DIR) $(TARGET)