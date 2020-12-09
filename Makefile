CC = gcc
CFLAGS = -Wall -Wextra -pedantic -g

SRC_DIR = src
OBJ_DIR = obj

TARGET = ifj20
MODULES = main scanner parser expression str stack symtable generator taclist builtin
SOURCES = $(MODULES:%=$(SRC_DIR)/%.c)
OBJS = $(MODULES:%=$(OBJ_DIR)/%.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS) $(SRC_DIR)/symtable.h
	$(CC) $(CFLAGS) $^ -o $@

clean: $(SRC_DIR)
	$(RM) -r $(OBJ_DIR)
	$(RM) $(TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) -c $< -o $@
