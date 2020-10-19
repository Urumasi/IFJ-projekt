CC = gcc
CFLAGS = -Wall -g

SRC_DIR = src
OBJ_DIR = obj

TARGET = ifj20c
MODULES = main
SOURCES = $(MODULES:%=$(SRC_DIR)/%.c)
OBJS = $(MODULES:%=$(OBJ_DIR)/%.o)

.PHONY: all

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	$(RM) -r $(OBJ_DIR)
	$(RM) $(TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) -c $< -o $@

