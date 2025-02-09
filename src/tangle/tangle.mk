SRCDIR := $(dir $(lastword $(MAKEFILE_LIST)))
TANGLE := $(SRCDIR)tangle
OBJS := $(patsubst %.c,%.o,$(wildcard $(SRCDIR)*.c))

$(TANGLE): $(OBJS)

include $(SRCDIR)../../nonrec.mk
