<<<<<<< Updated upstream
LIBO_TEST_BASENAMES = osc_expr_ast_test #osc_typetag_test #
=======
LIBO_TEST_BASENAMES = osc_match_test
>>>>>>> Stashed changes

INCLUDES = -I.. -I/System/Library/Frameworks/Carbon.framework/Headers -I/System/Library/Frameworks/CoreServices.framework/Headers
CFLAGS += -std=c99 -Wall -Wno-trigraphs -arch x86_64 -O3 -funroll-loops
CC = clang

all: $(LIBO_TEST_BASENAMES) ../libo.a

%: %.c
	$(CC) -O0 $(CFLAGS) $(INCLUDES) -framework CoreServices -L.. -lo -lfl -o $@ $@.c

.PHONY: clean
clean:
	rm -rf $(LIBO_TEST_BASENAMES)
