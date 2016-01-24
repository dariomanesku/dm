#
# Copyright 2016 Dario Manesku. All rights reserved.
# License: http://www.opensource.org/licenses/BSD-2-Clause
#

SILENT ?=

CC=g++
CFLAGS=-c -Iinclude -Wall -g
LDFLAGS=
SRCDIR=tests
BUILDDIR=_build
EXE=$(BUILDDIR)/dmtests

SRCS=$(SRCDIR)/main.cpp
OBJS=$(addprefix $(BUILDDIR)/, $(patsubst %.cpp, %.o, $(notdir $(SRCS))))

.PHONY: tests
tests: $(EXE)

-include $(wildcard $(BUILDDIR)/*.d)

$(BUILDDIR):
	$(SILENT)mkdir $(BUILDDIR)

$(EXE): $(BUILDDIR) $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) -MMD $< -o $@

.PHONY: all
all: tests

.PHONY: run
run: $(EXE)
	@./$(EXE)

.PHONY: gdb
gdb: $(EXE)
	gdb $(EXE)

.PHONY: clean
clean:
	-$(SILENT)rm -rf $(BUILDDIR)

# TODO: Write a better Makefile, look at the examples below.
#
# Example from internet:
#APP      = myapp
#
#OBJDIR   = .srcobjs
#
#SRCS    := $(shell find . -name '*.cpp')
#SRCDIRS := $(shell find . -name '*.cpp' -exec dirname {} \; | uniq)
#OBJS    := $(patsubst %.cpp,$(OBJDIR)/%.o,$(SRCS))
#DEPS     = $(patsubst %.cpp,$(OBJDIR)/%.d,$(SRCS))
#
#INCLUDES = -I./Include
#CXXFLAGS = -Wall -pedantic -ansi -c $(INCLUDES)
#LDFLAGS  =
#
#
#all: $(APP)
#
#$(APP) : buildrepo $(OBJS)
#        @echo "Building $@..."
#        @$(CXX) $(OBJS) $(LDFLAGS) -o $@
#
#$(OBJDIR)/%.o: %.cpp
#        @echo "Generating dependencies for $<..."
#        @$(call make-depend,$<,$@,$(subst .o,.d,$@))
#        @echo "Compiling $<..."
#        @$(CXX) $(CXXFLAGS) $< -o $@
#
#clean:
#        $(RM) -r $(OBJDIR)
#
#distclean: clean
#        $(RM) $(APP)
#
#buildrepo:
#        @$(call make-repo)
#
#define make-repo
#   for dir in $(SRCDIRS); \
#   do \
#        mkdir -p $(OBJDIR)/$$dir; \
#   done
#endef
#
#
## usage: $(call make-depend,source-file,object-file,depend-file)
#define make-depend
#  $(CXX) -MM            \
#         -MF $3         \
#         -MP            \
#         -MT $2         \
#         $(CXXFLAGS)    \
#         $(INCLUDES)    \
#         $1
#endef
#
#ifneq "$(MAKECMDGOALS)" "clean"
#-include $(DEPS)
#endif

# Another example from internet:
#CXX = clang++
#CXX_FLAGS = -Wfatal-errors -Wall -Wextra -Wpedantic -Wconversion -Wshadow
#
## Final binary
#BIN = mybin
## Put all auto generated stuff to this build dir.
#BUILD_DIR = ./build
#
## List of all .cpp source files.
#CPPS = main.cpp $(wildcard dir1/*.cpp) $(wildcard dir2/*.cpp)
#
## All .o files go to build dir.
#OBJ = $(CPP:%.cpp=$(BUILD_DIR)/%.o)
## Gcc/Clang will create these .d files containing dependencies.
#DEP = $(OBJ:%.o=%.d)
#
## Default target named after the binary.
#$(BIN) : $(BUILD_DIR)/$(BIN)
#
## Actual target of the binary - depends on all .o files.
#$(BUILD_DIR)/$(BIN) : $(OBJ)
#    # Create build directories - same structure as sources.
#    mkdir -p $(@D)
#    # Just link all the object files.
#    $(CXX) $(CXX_FLAGS) $^ -o $@
#
## Include all .d files
#-include $(DEP)
#
## Build target for every single object file.
## The potential dependency on header files is covered
## by calling `-include $(DEP)`.
#$(BUILD_DIR)/%.o : %.cpp
#    mkdir -p $(@D)
#    # The -MMD flags additionaly creates a .d file with
#    # the same name as the .o file.
#    $(CXX) $(CXX_FLAGS) -MMD -c $< -o $@
