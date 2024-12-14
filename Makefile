# Compiler and flags
CC = msp430-gcc
CFLAGS = -Wall -Wextra -std=c11 -I.

# Tools and external commands
FORMAT_TOOL = clang-format-12
CCHECK_TOOL = cppcheck

# Directories
SRCDIR = APP
OBJDIR = obj
BINDIR = bin

# Subdirectories for modules
SUBDIRS = $(shell find $(SRCDIR) -type d)

# Source files
SRCS = $(foreach dir,$(SUBDIRS),$(wildcard $(dir)/*.c))

# Object files
OBJS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))

# Executables
EXEC = $(BINDIR)/main

# Default target
all: setup $(EXEC)

# Link object files into the final executable
$(EXEC): $(OBJS)
	@mkdir -p $(BINDIR)
	$(CC) $(OBJS) -o $@

# Compile source files to object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Create necessary directories
setup:
	@mkdir -p $(OBJDIR) $(BINDIR)

# Clean up build files
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# Format source code
format:
	@echo "Formatting source code..."
	@find $(SRCDIR) -name '*.c' -o -name '*.h' | xargs $(FORMAT_TOOL) -i

# Static code analysis
ccheck:
	@echo "Running static code analysis..."
	@$(CCHECK_TOOL) --enable=all --std=c11 --inconclusive --quiet $(SRCS)

# Phony targets
.PHONY: all clean setup format ccheck
