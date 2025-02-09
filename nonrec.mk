# Dead-simple setup for nonrecursive make.
# Use this include statement at the top of every file.mk
#   SRCDIR := $(dir $(lastword $(MAKEFILE_LIST)))
# And include this at the bottom of every file.mk
#   include $(SRCDIR)....../nonrec.mk
# (change .... path if needed)

ifndef nonrec
	nonrec := 1 # include guard

fmt:
	find . -name '*.c' \
		-o -name '*.h' \
		-o -name '*.cpp' \
		-o -name '*.hpp' | xargs clang-format -i

# Double colon allows multiple definitions for the clean:: target
clean::
	find . -name '*.o' | xargs $(RM)

# EXTRA_CFLAGS is set in some file.mk
%.o: %.c
	$(CC) -c $< -o $@ $(EXTRA_CFLAGS) $(CFLAGS)

.PHONY: clean fmt

endif
