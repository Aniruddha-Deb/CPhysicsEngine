# A simple Makefile for compiling small SDL projects

# set the compiler
CC := gcc

# set the compiler flags
CFLAGS := -std=c99 -Wall -lSDL2 -lm

all:
	$(CC) ballpit.c -o exec/ballpit $(CFLAGS)
