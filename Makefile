SRC_DIR   = src
INC_DIR   = include
TEST_DIR  = test
BUILD_DIR = build
LIB_DIR   = $(BUILD_DIR)/lib
BIN_DIR   = $(BUILD_DIR)/bin
OBJ_DIR   = $(BUILD_DIR)/obj
GTEST_DIR = extsrc/gtest-1.7.0


# -----------------------------------------------------------------------------
# libhime
# -----------------------------------------------------------------------------

UNAME := $(shell uname -s)
ifeq ($(UNAME),Darwin)
		CXX=/usr/bin/clang++
endif
CXXFLAGS = -g -MMD -MP -Wall -Wextra -pthread -std=c++1y
INCS += -I$(INC_DIR)

SRCS     = $(shell find $(SRC_DIR) -name '*.cpp')
OBJS     = $(addprefix $(OBJ_DIR)/,$(patsubst $(SRC_DIR)/%,%,$(SRCS:.cpp=.o)))
OBJ_DIRS = $(dir $(OBJS))
DEPS    += $(OBJS:.o=.d)

TARGET   = $(LIB_DIR)/libhime.a

default: $(TARGET)
.PHONY: default

$(TARGET): $(OBJS)
	@[ -d $(LIB_DIR) ] || mkdir -p $(LIB_DIR)
	$(AR) ruc $(TARGET) $(OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@[ -d $(dir $@) ] || mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCS) -o $@ -c $<


# -----------------------------------------------------------------------------
# Test
# -----------------------------------------------------------------------------

TEST_SRCS = $(shell find $(TEST_DIR) -name '*.cpp')
TEST_OBJS  = $(addprefix $(OBJ_DIR)/, $(notdir $(TEST_SRCS:.cpp=.o)))
DEPS += $(TEST_OBJS:.o=.d)
LIBS += -L$(LIB_DIR)
LIBS += -lhime -lyaml-cpp
TEST_TARGET = $(BIN_DIR)/gtest_hime

CPPFLAGS += -isystem $(GTEST_DIR)/include
#CXXFLAGS = -g -Wall -Wextra -pthread

# All Google Test headers.  Usually you shouldn't change this
# definition.
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
								$(GTEST_DIR)/include/gtest/internal/*.h
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

# For simplicity and to avoid depending on Google Test's
# implementation details, the dependencies specified below are
# conservative and not optimized.  This is fine as Google Test
# compiles fast and for ordinary users its source rarely changes.

$(OBJ_DIR)/gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
			-o $@ $(GTEST_DIR)/src/gtest-all.cc

$(OBJ_DIR)/gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
			-o $@ $(GTEST_DIR)/src/gtest_main.cc

$(LIB_DIR)/gtest.a : $(OBJ_DIR)/gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

$(LIB_DIR)/gtest_main.a : $(OBJ_DIR)/gtest-all.o $(OBJ_DIR)/gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

test: $(TEST_TARGET)
.PHONY: test

$(TEST_TARGET): $(TARGET) $(TEST_OBJS) $(LIB_DIR)/gtest_main.a
	@[ -d $(BIN_DIR) ] || mkdir -p $(BIN_DIR)
	$(CXX) $(LDFLAGS) -o $@ $(TEST_OBJS) \
			$(LIB_DIR)/gtest_main.a $(LIBS) -lpthread


$(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp $(GTEST_HEADERS)
	@[ -d $(OBJ_DIR) ] || mkdir -p $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(INCS) -o $@ -c $<

# -----------------------------------------------------------------------------
# js bindings
# -----------------------------------------------------------------------------

js: $(TARGET) bindings/embind.cpp
	emcc --bind -std=c++1y $(INCS) -o build/hime.js $(SRCS) bindings/embind.cpp
.PHONY: js

clean:
	rm -rf $(BUILD_DIR)
.PHONY: clean

-include $(DEPS)

