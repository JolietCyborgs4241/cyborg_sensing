#	autonomous library Makefile

CC = gcc
INC     = -I../include
CFLAGS = -fPIC -Wall -Wextra -O2 -g $(INC)
LDFLAGS = -shared  # linking flags
RM = rm -f  # rm command
TARGET_LIB = libcyborgauto.so # target lib

SRCS = memory.c serial.c
OBJS = $(SRCS:.c=.o)


.PHONY: all

all: ${TARGET_LIB}

$(TARGET_LIB): $(OBJS)
	$(CC) ${LDFLAGS} -o $@ $^

$(SRCS:.c=.d):%.d:%.c
	$(CC) $(CFLAGS) -MM $< >$@

include $(SRCS:.c=.d)

.PHONY: clean
clean:
	-${RM} ${TARGET_LIB} ${OBJS} $(SRCS:.c=.d)


