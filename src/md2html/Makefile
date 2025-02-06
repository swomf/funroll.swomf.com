PROG = md2html
SRCS = md4c.c entity.c cmdline.c md4c-html.c
OBJS = $(SRCS:.c=.o)

# CFLAGS adapted from CMakeLists.txt in upstream mity/md4c's root dir.
# | DIFFERENCES FROM UPSTREAM: -O1 is used instead of -O3 (see benchmark)
# |                            -fPIC is not used (flag added by cmake)
# User can append to CFLAGS if desired.
override CFLAGS := -DMD_VERSION_MAJOR=0 -DMD_VERSION_MINOR=5 -DMD_VERSION_RELEASE=2 \
				 -Wall -Wextra -Wshadow -Wdeclaration-after-statement -O1 $(CFLAGS)

all: $(PROG)

$(PROG): $(OBJS)

clean:
	rm -f $(OBJS) $(PROG)

fmt:
	# Just in case a dev runs `make fmt` in the current dir
	clang-format -i *.c *.h

.PHONY: all clean fmt
