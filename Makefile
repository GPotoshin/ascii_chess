.PHONY: deps

CCFLAGS += -I./deps
LDFLAGS += -lncurses

all: bin/chess

bin/chess: src/main.c
	cc $(CCFLAGS) $(LDFLAGS) -o $@ $<

deps:
	sh install_deps.sh

clear:
	rm bin/*

clear_deps:
	rm -rd deps/*
