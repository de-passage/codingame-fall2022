# Thanks to Job Vranish (https://spin.atomicobject.com/2016/08/26/makefile-c-projects/)
TARGET_EXEC := final_program

OPPONENT ?= basic

BUILD_DIR := ./build
SRC_DIRS := ./src

# Find all the C and C++ files we want to compile
# Note the single quotes around the * expressions. Make will incorrectly expand these otherwise.
GENERATED_DIR := ./generated
GENERATED_SRCS := $(GENERATED_DIR)/generated.cpp

# String substitution for every C/C++ file.
# As an example, hello.cpp turns into ./build/hello.cpp.o
GENERATED_OBJS := $(GENERATED_SRCS:%=$(BUILD_DIR)/%.o)

TARGET_PATH := $(BUILD_DIR)/$(TARGET_EXEC)

# String substitution (suffix version without %).
# As an example, ./build/hello.cpp.o turns into ./build/hello.cpp.d
DEPS := $(OBJS:.o=.d)

# Every folder in ./src will need to be passed to GCC so that it can find header files
INC_DIRS := $(shell find $(SRC_DIRS) -type d)
# Add a prefix to INC_DIRS. So moduleA would become -ImoduleA. GCC understands this -I flag
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# The -MMD and -MP flags together generate Makefiles for us!
# These files will have .d instead of .o as the output.
CPPFLAGS := $(INC_FLAGS)

SRC_DEPS := $(shell find $(SRC_DIRS) -name '*.cpp' -or -name '*.c' -or -name '*.s')

build: $(BUILD_DIR)/$(TARGET_EXEC)
	$(MAKE) $(GENERATED_SRCS)

test: build
	./scripts/launcher.bash -p1 $(TARGET_PATH) -p2 ais/$(OPPONENT) -s -t 8 -n 12

# The final build step.
$(BUILD_DIR)/$(TARGET_EXEC): $(GENERATED_OBJS)
	$(CXX) $(GENERATED_OBJS) -o $@ $(LDFLAGS)

# Build step for C source
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Build step for C++ source
$(BUILD_DIR)/%.cpp.o: %.cpp $(GENERATED_SRCS)
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(GENERATED_SRCS):
	mkdir -p $(GENERATED_DIR)
	../combine/combine.bash -o $(GENERATED_SRCS) src/main.cpp $(INC_FLAGS)

.PHONY: clean $(GENERATED_SRCS) build release test
clean:
	rm -r $(BUILD_DIR)

# Include the .d makefiles. The - at the front suppresses the errors of missing
# Makefiles. Initially, all the .d files will be missing, and we don't want those
# errors to show up.
-include $(DEPS)
