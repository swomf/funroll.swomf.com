SRCDIR := $(dir $(lastword $(MAKEFILE_LIST)))
MD2HTML := $(SRCDIR)md2html
OBJS := $(patsubst %.c,%.o,$(wildcard $(SRCDIR)*.c))

# CFLAGS adapted from CMakeLists.txt in upstream mity/md4c's root dir.
# | DIFFERENCES FROM UPSTREAM: -O1 is used instead of -O3 (see benchmark)
# |                            -fPIC is not used (flag added by cmake)
# User can append to CFLAGS if desired.
EXTRA_CFLAGS := \
	-DMD_VERSION_MAJOR=0 -DMD_VERSION_MINOR=5 -DMD_VERSION_RELEASE=2 \
	-Wall -Wextra -Wshadow -Wdeclaration-after-statement -O1

$(MD2HTML): $(OBJS)

include $(SRCDIR)../../nonrec.mk
