# Copyright (C) 2020 the Massachusetts Institute of Technology
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

###########################
# WHEN IN DOUBT: make clean
###########################

### Default Target ###
all: rotate
######################

### Default Flags ###   DO NOT MODIFY
# DO NOT MODIFY THESE IN THIS FILE : Instead, run with `make LOCAL=0 DEBUG=1` for example.

# You will be graded with the default values

## Set to 0 if you want to compile for awsrun. This is how we will grade you.
## Set to 1 if you want to compile for your native machine
LOCAL := 0

# Set to 1 if you want to compile in debug mode
DEBUG := 0
#####################

### Compiler Settings ###

# DO NOT MODIFY THE COMPILER
# You will be graded with the clang compiler that ships with the course VM
CC := clang

# You can modify these flags if you know what to do.
CFLAGS := -Wall -ftree-vectorize -flto
LDFLAGS := -Wall -flto
#########################

### Dependency Declarations ###
# Make sure to add all your header file dependencies here
DEPS := ../utils/libbmp.h ../utils/tester.h ../utils/utils.h

# Make sure to add all your object file dependencies here
# If you create a file under project1/snailspeed/x.c you want to add x.o here.
OBJ := ../utils/libbmp.o ../utils/tester.o ../utils/utils.o ../utils/main.o rotate.o
###############################

### Adjust CFLAGS ###
# Uses the Makefile flags to adjust CFLAGS

# DO NOT MODIFY THE ARCHITECTURES
ifeq ($(LOCAL),0)
	# You will be graded on haswell.
	CFLAGS += -march=haswell
else
	CFLAGS += -march=native
endif

ifeq ($(DEBUG),0)
	CFLAGS += -O3
else
	CFLAGS += -Og -ggdb3 -DDEBUG
	LDFLAGS += -ggdb3
endif
#####################

### Flag Recompile Management ###   DO NOT MODIFY

# Make sure the .buildmode file contains the relevant Makefile flags.
# Compiling recipes depend on .buildmode so that they recompile if you change the Makefile flags.
OLDMODE := $(shell cat .buildmode 2> /dev/null)
BUILDMODE_STR := $(LOCAL) $(DEBUG)

ifneq ($(OLDMODE),$(BUILDMODE_STR))
$(shell echo $(LOCAL) $(DEBUG) > .buildmode)
endif
#################################

### Compilation and Linking Rules ###   DO NOT MODIFY

# Rule to compile any .c file to a .o file
%.o: %.c $(DEPS) .buildmode
	$(CC) -c -o $@ $< $(CFLAGS)

# Rule to link the rotate binary
rotate: $(OBJ) .buildmode Makefile
	$(CC) -o $@ $(OBJ) $(LDFLAGS)
#####################################

### Printed Warnings ###   DO NOT MODIFY
warn_flags:
	@printf "\033[01;33mBE ADVISED: You have selected to build for your native architecture. This might be different than Haswell, which the awsrun grading machines use.\033[00m\n"
	@printf "\033[01;33mUnless you have a good reason, unset the LOCAL flag or use LOCAL=0 before getting performance numbers on awsrun.\033[00m\n"
	@printf "\033[01;33mawsrun is the only way you should be running performance measurements.\033[00m\n"

ifeq ($(LOCAL),1)
rotate: warn_flags
endif
########################

.PHONY: clean warn_flags all

clean:
	rm -f ../utils/*.o
	rm -f *.o rotate
	rm -f $(OBJS)
