PROG = md2html
SRCS = md4c.c entity.c cmdline.c md4c-html.c
OBJS = $(SRCS:.c=.o)

# CFLAGS adapted from CMakeLists.txt in upstream mity/md4c's root dir.
CFLAGS = -DMD_VERSION_MAJOR=0 -DMD_VERSION_MINOR=5 -DMD_VERSION_RELEASE=2 \
				 -Wall -Wextra -Wshadow -Wdeclaration-after-statement

all: $(PROG)

$(PROG): $(OBJS)

clean:
	rm -f $(OBJS) $(PROG)

fmt:
	# Just in case a dev runs `make fmt` in the current dir
	clang-format -i *.c *.h

.PHONY: all clean fmt
