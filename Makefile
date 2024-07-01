CXX = g++
CXXFLAGS = -Wall -Wextra -O2
LIB_NAME = lcptools
SRC = string.cpp
OBJ = $(SRC:.cpp=.o)
PREFIX ?= /usr/local
INCLUDE_DIR = $(PREFIX)/include/$(LIB_NAME)
LIB_DIR = $(PREFIX)/lib

.PHONY: all clean install uninstall

all: lib$(LIB_NAME).a

lib$(LIB_NAME).a: $(OBJ)
	ar rcs $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

install: lib$(LIB_NAME).a
	@mkdir -p $(INCLUDE_DIR)
	@cp $(SRC) $(INCLUDE_DIR)/
	@mkdir -p $(LIB_DIR)
	@cp lib$(LIB_NAME).a $(LIB_DIR)
	@echo "Library installed successfully."
	@rm lib$(LIB_NAME).a
	@rm *.o

uninstall:
	@echo "Cleaning."
	@rm -f $(INCLUDE_DIR)/$(SRC)
	@rm -f $(LIB_DIR)/lib$(LIB_NAME).a
	@echo "Library uninstalled successfully."

clean:
	@rm -f $(OBJ) lib$(LIB_NAME).a
