# g++
CXX = g++
CXXFLAGS = -std=c++11 -O2 -Wall -Wextra
CXXEXTRA = -fPIC

# archiver and flags
AR = ar
ARFLAGS = rcs

# variables
SRC = lps.cpp base_core.cpp core.cpp encoding.cpp
HDR = lps.h base_core.h core.h encoding.h
OTHER_HDR = constant.h
OBJ_STATIC = $(SRC:.cpp=_s.o)
OBJ_DYNAMIC = $(SRC:.cpp=_d.o)

# library names
LIB_NAME = lcptools
STATIC = lib$(LIB_NAME).a
DYNAMIC = lib$(LIB_NAME).so

PREFIX ?= /usr/local
INCLUDE_DIR = $(PREFIX)/include
LIB_DIR = $(PREFIX)/lib

.PHONY: all clean install uninstall

all:
	@echo ""

install: $(STATIC) $(DYNAMIC)
	@echo "";
	@echo "WARNING! Please make sure that $(LIB_DIR) included in LD_LIBRARY_PATH";
	@echo "If not, you can export it as: ";
	@echo "    export LD_LIBRARY_PATH=$(LIB_DIR):\$$LD_LIBRARY_PATH";
	@echo "    sudo ldconfig";
	@echo "";

uninstall:
	rm -f $(LIB_DIR)/$(STATIC)
	rm -f $(LIB_DIR)/$(DYNAMIC)
	@for hdr in $(HDR); do \
		echo "Removing $(INCLUDE_DIR)/$$hdr;"; \
		rm -f $(INCLUDE_DIR)/$$hdr; \
	done
	@for hdr in $(OTHER_HDR); do \
		echo "Removing $(INCLUDE_DIR)/$$hdr;"; \
		rm -f $(INCLUDE_DIR)/$$hdr; \
	done

clean:
	rm -f $(STATIC) $(DYNAMIC) $(OBJ_STATIC) $(OBJ_DYNAMIC)

# target for static library
$(STATIC): $(OBJ_STATIC)
	$(AR) $(ARFLAGS) $@ $^
	rm -f $(OBJ_STATIC)
	mv $@ $(LIB_DIR)
	cp $(HDR) $(OTHER_HDR) $(INCLUDE_DIR)

# target for dynamic library
$(DYNAMIC): $(OBJ_DYNAMIC)
	$(CXX) -shared -o $@ $^
	rm -f $(OBJ_DYNAMIC)
	mv $@ $(LIB_DIR)
	cp $(HDR) $(OTHER_HDR) $(INCLUDE_DIR)

# rule to compile .cpp files to .o files for static library
%_s.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# rule to compile .cpp files to .o files for dynamic library
%_d.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $(CXXEXTRA) $< -o $@

# dependencies
lps.o: lps.cpp lps.h core.h base_core.h encoding.h constant.h
core.o: core.cpp core.h base_core.h constant.h
base_core.o: base_core.cpp base_core.h encoding.h constant.h
encoding.o: encoding.cpp encoding.h