CC = gcc
CFLAGS = -Wall -Wextra -pedantic -g

OBJ_DIR = obj

TARGET = ifj20
MODULES = main scanner parser expression str stack symtable generator
SOURCES = $(MODULES:%=%.c)
OBJS = $(MODULES:%=$(OBJ_DIR)/%.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS) symtable.h
	$(CC) $(CFLAGS) $^ -o $@

clean:
	$(RM) -r $(OBJ_DIR)
	$(RM) $(TARGET)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) -c $< -o $@
