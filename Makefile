# Sapphire Makefile
# Quick build system (use CMake for full build)

CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O2 -I./src
LDFLAGS = -lpthread

# Check for LLVM (optional for now)
LLVM_CONFIG := $(shell command -v llvm-config 2> /dev/null)
ifdef LLVM_CONFIG
    LLVM_CXXFLAGS := $(shell llvm-config --cxxflags)
    LLVM_LDFLAGS := $(shell llvm-config --ldflags --libs core support irreader passes target x86)
    CXXFLAGS += $(LLVM_CXXFLAGS) -DHAVE_LLVM
    LDFLAGS += $(LLVM_LDFLAGS)
    LLVM_STATUS = "✓ LLVM support enabled"
else
    LLVM_STATUS = "⚠ LLVM not found - code generation disabled"
endif

TARGET = sapp
ALIAS = sapphire
SRCDIR = src
OBJDIR = build/obj

# Source files
SOURCES = $(SRCDIR)/main.cpp \
          $(SRCDIR)/lexer/lexer.cpp \
          $(SRCDIR)/lexer/token.cpp \
          $(SRCDIR)/parser/parser.cpp \
          $(SRCDIR)/parser/expr.cpp \
          $(SRCDIR)/parser/stmt.cpp \
          $(SRCDIR)/interpreter/interpreter.cpp \
          $(SRCDIR)/types/type.cpp \
          $(SRCDIR)/semantic/type_checker.cpp \
          $(SRCDIR)/semantic/type_inference.cpp

# Add codegen if LLVM is available
ifdef LLVM_CONFIG
    SOURCES += $(SRCDIR)/codegen/llvm_codegen.cpp \
               $(SRCDIR)/codegen/codegen_impl.cpp
endif

OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

.PHONY: all build test install clean help run

all: help

help:
	@echo "Sapphire Build System"
	@echo ""
	@echo "Targets:"
	@echo "  make quick      - Quick build (no CMake)"
	@echo "  make run        - Build and run hello.spp"
	@echo "  make clean      - Clean build artifacts"
	@echo ""
	@echo "Status:"
	@echo "  $(LLVM_STATUS)"
	@echo ""
	@echo "For full build, use: ./scripts/build.sh"

quick: $(TARGET)
	@ln -sf $(TARGET) $(ALIAS)
	@echo "✓ Build complete: ./$(TARGET)"
	@echo "  Alias created: ./$(ALIAS) -> ./$(TARGET)"
	@echo ""
	@echo "$(LLVM_STATUS)"

$(TARGET): $(OBJECTS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)
	mkdir -p $(OBJDIR)/lexer
	mkdir -p $(OBJDIR)/parser
	mkdir -p $(OBJDIR)/interpreter
	mkdir -p $(OBJDIR)/types
	mkdir -p $(OBJDIR)/semantic
	mkdir -p $(OBJDIR)/error
	mkdir -p $(OBJDIR)/codegen

run: quick
	@echo "Running examples/hello.spp..."
	@./sapp examples/hello.spp

clean:
	rm -rf $(OBJDIR) $(TARGET) $(ALIAS)
	rm -rf build/
