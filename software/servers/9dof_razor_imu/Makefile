#	autonomous 9dof Makefile


CV_INC_DIR	= ../../include
INC		= -I$(CV_INC_DIR)
CFLAGS	+= $(INC) -pthread -g $(DEBUG)

LDLIBS  = -lcyborgauto
LDFLAGS	+= -L ../../lib -pthread

SRCS	= init.c globals.c

OBJS	= ${SRCS:.c=.o}

TARGETS	= 9dof_server



all:	$(TARGETS)



9dof_server:	$(OBJS)

init.o:		init.c $(CV_INC_DIR)/cv.h $(CV_INC_DIR)/cv_net.h \
			$(CV_INC_DIR)/9dof/externs.h

clean:
	rm -rf $(OBJS) $(TARGETS)


valgrind:	cv_db
	valgrind --leak-check=full --show-leak-kinds=all --read-var-info=yes -v ./9dof_server -c $(PORT) -s 115200 -D 2 -d DEBUG 2>stderr

