CC = gcc
CFLAGS = -Wall -Wextra -pedantic -g

SRC_DIR = src
OBJ_DIR = obj
LIB_DIR = lib

TARGET = ifj20c
MODULES = main
SOURCES = $(MODULES:%=$(SRC_DIR)/%.c)
OBJS = $(MODULES:%=$(OBJ_DIR)/%.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS) $(LIB_DIR)/libhtab.a
	$(CC) $(CFLAGS) $^ -o $@

clean: $(SRC_DIR)/htab/Makefile
	$(RM) -r $(OBJ_DIR)
	$(RM) -r $(LIB_DIR)
	$(RM) $(TARGET)
	cd $(SRC_DIR)/htab && $(MAKE) clean

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) -c $< -o $@

$(LIB_DIR)/libhtab.a: $(SRC_DIR)/htab/Makefile
	@mkdir -p $(@D)
	cd $(SRC_DIR)/htab && $(MAKE)
	cp $(SRC_DIR)/htab/libhtab.a $(LIB_DIR)

