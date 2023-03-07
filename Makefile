CCFLAGS += -I./deps
LDFLAGS += -lncurses

bin/chess: src/main.c
	cc $(CCFLAGS) $(LDFLAGS) -o $@ $<
