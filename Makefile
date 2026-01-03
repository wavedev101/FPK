CC ?= gcc
CFLAGS ?= -O2 -Wall
BUILDDIR := build
TARGET := fpk64
EXEEXT := $(if $(filter Windows_NT,$(OS)),.exe,)

SRCS := fpk.c fpk_main.c
OBJS := $(SRCS:%.c=$(BUILDDIR)/%.o)

all: $(BUILDDIR)/$(TARGET)$(EXEEXT)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/%.o: %.c fpk.h | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/$(TARGET)$(EXEEXT): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf $(BUILDDIR)

.PHONY: all clean
