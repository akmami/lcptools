# g++
CXX = g++
CXXFLAGS = -std=c++11 -O2 -Wall -Wextra
CXXEXTRA = -fPIC

ifdef STATS
    CXXFLAGS += -DSTATS
endif

# archiver and flags
AR = ar
ARFLAGS = rcs

# variables
SRC = $(wildcard *.cpp)
HDR = $(SRC:.cpp=.h)
OTHER_HDR = constant.h
OBJ_STATIC = $(SRC:.cpp=_s.o)
OBJ_DYNAMIC = $(SRC:.cpp=_d.o)

# test files
TEST_DIR = tests
TEST_SRC = $(wildcard $(TEST_DIR)/*.cpp)
TESTS = $(patsubst $(TEST_DIR)/%.cpp,%,$(TEST_SRC))
TEST_OBJS = $(TEST_SRC:.cpp=.o)

# library names
LIB_NAME = lcptools
STATIC = lib$(LIB_NAME).a
DYNAMIC = lib$(LIB_NAME).so

PREFIX ?= /usr/local
INCLUDE_DIR = $(PREFIX)/include
LIB_DIR = $(PREFIX)/lib

.PHONY: all clean install uninstall

install: $(STATIC) $(DYNAMIC)
	mkdir -p $(INCLUDE_DIR)
	cp $(OTHER_HDR) $(HDR) $(INCLUDE_DIR)
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
	rm -f $(TEST_OBJS) 
	@echo "rm $(LIB_DIR)/$(STATIC)";
	@if [ -f "$(LIB_DIR)/$(STATIC)" ]; then \
		rm $(LIB_DIR)/$(STATIC) 2>/tmp/lcptools.err || \
			{ \
				echo "Couldn't remove $(LIB_DIR)/$(STATIC)"; \
				exit 1; \
			}; \
	fi
	@echo "rm $(LIB_DIR)/$(DYNAMIC)";
	@if [ -f "$(LIB_DIR)/$(DYNAMIC)" ]; then \
		rm $(LIB_DIR)/$(DYNAMIC) 2>/tmp/lcptools.err || \
			{ \
				echo "Couldn't remove $(LIB_DIR)/$(DYNAMIC)"; \
				exit 1; \
			}; \
	fi
	rm -f $(OBJ_STATIC) $(OBJ_DYNAMIC) 

# target for static library
$(STATIC): $(OBJ_STATIC)
	$(AR) $(ARFLAGS) $@ $^
	rm -f $(OBJ_STATIC)
	mkdir -p $(LIB_DIR)
	@echo "mv $@ $(LIB_DIR)  2>/tmp/lcptools.err"
	@mv $@ $(LIB_DIR) 2>/tmp/lcptools.err || \
		{ \
			echo "Couldn't move $@ to $(LIB_DIR)"; \
			exit 1; \
		}

# target for dynamic library
$(DYNAMIC): $(OBJ_DYNAMIC)
	$(CXX) -shared -o $@ $^
	rm -f $(OBJ_DYNAMIC)
	mkdir -p $(LIB_DIR)
	@echo "mv $@ $(LIB_DIR)  2>/tmp/lcptools.err"
	@mv $@ $(LIB_DIR)  2>/tmp/lcptools.err || \
		{ \
			echo "Couldn't move $@ to $(LIB_DIR)"; \
			exit 1; \
		}

# rule to compile .cpp files to .o files for static library
%_s.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# rule to compile .cpp files to .o files for dynamic library
%_d.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $(CXXEXTRA) $< -o $@

# dependencies
lps.o: lps.cpp lps.h core.h encoding.h constant.h
core.o: core.cpp core.h encoding.h constant.h
encoding.o: encoding.cpp encoding.h

# compile and link test executables
$(TEST_DIR)/%.o: $(TEST_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(TESTS): %: $(TEST_DIR)/%.o
	$(CXX) $(CXXFLAGS) -o tests/$@ $< -L$(LIB_DIR) -l$(LIB_NAME) -Wl,-rpath,$(LIB_DIR)

# run all tests
test: $(TESTS)
	@for test in $(TESTS); do \
		tests/$$test || exit 1; \
		rm tests/$${test}.o; \
		rm tests/$$test; \
	done

