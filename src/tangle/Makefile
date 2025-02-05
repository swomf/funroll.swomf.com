PROG = tangle
CFLAGS = -Wall -Wextra -Wshadow

$(PROG): tangle.c

clean:
	$(RM) $(PROG)

fmt:
	# Just in case a dev runs `make fmt` in the current dir
	clang-format -i *.c *.h

.PHONY: all clean fmt
