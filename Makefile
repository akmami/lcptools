# g++
CXX = g++
CXXFLAGS = -std=c++11 -O3 -Wall -Wextra
CXXEXTRA = -fPIC

# archiver and flags
AR = ar
ARFLAGS = rcs

# variables
SRC = encoding.cpp hash.cpp core.cpp lps.cpp
HDR = $(SRC:.cpp=.h)
OTHER_HDR = constant.h rules.h
OBJ_STATIC_STATS = $(SRC:.cpp=_s_stats.o)
OBJ_STATIC = $(SRC:.cpp=_s.o)
OBJ_DYNAMIC_STATS = $(SRC:.cpp=_d_stats.o)
OBJ_DYNAMIC = $(SRC:.cpp=_d.o)

# test files
TEST_DIR = tests
TESTS = $(patsubst $(TEST_DIR)/%.cpp,%,$(wildcard $(TEST_DIR)/*.cpp))

# library names
LIB_NAME = lcptools
STATIC_STATS = lib$(LIB_NAME)S.a
STATIC = lib$(LIB_NAME).a
DYNAMIC_STATS = lib$(LIB_NAME)S.so
DYNAMIC = lib$(LIB_NAME).so

PREFIX ?= /usr/local
ABS_PREFIX := $(realpath $(PREFIX))
INCLUDE_DIR = $(ABS_PREFIX)/include
LIB_DIR = $(ABS_PREFIX)/lib

.PHONY: all clean install uninstall test

install: clean $(STATIC_STATS) $(STATIC) $(DYNAMIC_STATS) $(DYNAMIC) lcptools

	mkdir -p $(INCLUDE_DIR)
	rm -f *.o
	cp $(OTHER_HDR) $(HDR) $(INCLUDE_DIR)
	@echo "";
	@echo "WARNING! Please make sure that $(LIB_DIR) included in LD_LIBRARY_PATH";
	@echo "";

uninstall:
	rm -f lcptools;
	rm -f $(LIB_DIR)/$(STATIC_STATS)
	rm -f $(LIB_DIR)/$(STATIC)
	rm -f $(LIB_DIR)/$(DYNAMIC_STATS)
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
	rm -f $(TEST_DIR)/*.o 
	@echo "rm $(LIB_DIR)/$(STATIC_STATS)";
	@if [ -f "$(LIB_DIR)/$(STATIC_STATS)" ]; then \
		rm $(LIB_DIR)/$(STATIC_STATS) 2>/tmp/lcptools.err || \
			{ \
				echo "Couldn't remove $(LIB_DIR)/$(STATIC_STATS)"; \
				exit 1; \
			}; \
	fi
	@echo "rm $(LIB_DIR)/$(STATIC)";
	@if [ -f "$(LIB_DIR)/$(STATIC)" ]; then \
		rm $(LIB_DIR)/$(STATIC) 2>/tmp/lcptools.err || \
			{ \
				echo "Couldn't remove $(LIB_DIR)/$(STATIC)"; \
				exit 1; \
			}; \
	fi
	@echo "rm $(LIB_DIR)/$(DYNAMIC_STATS)";
	@if [ -f "$(LIB_DIR)/$(DYNAMIC_STATS)" ]; then \
		rm $(LIB_DIR)/$(DYNAMIC_STATS) 2>/tmp/lcptools.err || \
			{ \
				echo "Couldn't remove $(LIB_DIR)/$(DYNAMIC_STATS)"; \
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
	rm -f $(OBJ_STATIC_STATS) $(OBJ_STATIC) 
	rm -f $(OBJ_DYNAMIC_STATS) $(OBJ_DYNAMIC) 
	rm -f lcptools

# target for static library with STATS
$(STATIC_STATS): $(OBJ_STATIC_STATS)
	$(AR) $(ARFLAGS) $@ $^
	rm -f $(OBJ_STATIC_STATS)
	mkdir -p $(LIB_DIR)
	@mv $@ $(LIB_DIR) || \
		{ \
			echo "Couldn't move $@ to $(LIB_DIR)"; \
			exit 1; \
		}

# target for static library without STATS
$(STATIC): $(OBJ_STATIC)
	$(AR) $(ARFLAGS) $@ $^
	rm -f $(OBJ_STATIC)
	mkdir -p $(LIB_DIR)
	@mv $@ $(LIB_DIR) || \
		{ \
			echo "Couldn't move $@ to $(LIB_DIR)"; \
			exit 1; \
		}

# target for dynamic library with STATS
$(DYNAMIC_STATS): $(OBJ_DYNAMIC_STATS)
	$(CXX) -shared -o $@ $^
	rm -f $(OBJ_DYNAMIC_STATS)
	mkdir -p $(LIB_DIR)
	@mv $@ $(LIB_DIR) || \
		{ \
			echo "Couldn't move $@ to $(LIB_DIR)"; \
			exit 1; \
		}

# target for dynamic library without STATS
$(DYNAMIC): $(OBJ_DYNAMIC)
	$(CXX) -shared -o $@ $^
	rm -f $(OBJ_DYNAMIC)
	mkdir -p $(LIB_DIR)
	@mv $@ $(LIB_DIR) || \
		{ \
			echo "Couldn't move $@ to $(LIB_DIR)"; \
			exit 1; \
		}

# target to compile lcptools executable
lcptools: $(SRC) $(HDR) $(OTHER_HDR)
	rm -f $@
	$(CXX) $(CXXFLAGS) -o $@ $@.cpp $(SRC)
	chmod +x $@

# rule to compile .cpp files to .o files for static library with STATS
%_s_stats.o: %.cpp
	$(CXX) $(CXXFLAGS) -DSTATS -c $< -o $@

# rule to compile .cpp files to .o files for static library without STATS
%_s.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# rule to compile .cpp files to .o files for dynamic library with STATS
%_d_stats.o: %.cpp
	$(CXX) $(CXXFLAGS) -DSTATS -c $(CXXEXTRA) $< -o $@

# rule to compile .cpp files to .o files for dynamic library without STATS
%_d.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $(CXXEXTRA) $< -o $@

# run all tests
test:
	@echo "Running tests..."
	@for test in $(TESTS); do \
		echo "Compiling $$test.cpp..."; \
		$(CXX) $(CXXFLAGS) -DSTATS -I$(INCLUDE_DIR) -o tests/$$test tests/$$test.cpp -L$(LIB_DIR) -l$(LIB_NAME)S -Wl,-rpath,$(LIB_DIR); \
		if [ $$? -ne 0 ]; then \
			echo "Compilation failed for $$test.cpp"; \
			exit 1; \
		fi; \
		tests/$$test || exit 1; \
		rm -f tests/$$test; \
	done
	@echo "All tests passed."
