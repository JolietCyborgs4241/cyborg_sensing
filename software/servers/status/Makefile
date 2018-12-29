#	status server Makefile


CV_INC_DIR	= ../../include
INC		= -I$(CV_INC_DIR)
CFLAGS	+= $(INC) -pthread -g $(DEBUG)

LDLIBS = -lcyborgauto
LDFLAGS	+= -L ../../lib -pthread

SRCS	= cyborg_status.c init.c globals.c

OBJS	= ${SRCS:.c=.o}

TARGETS	= cyborg_status


all:	$(TARGETS)


cyborg_status:	$(OBJS)

cyborg_status.o:	cyborg_status.c $(CV_INC_DIR)/cv.h $(CV_INC_DIR)/cv_net.h \
			$(CV_INC_DIR)/status/externs.h $(CV_INC_DIR)/status/status.h

init.o:		init.c $(CV_INC_DIR)/cv.h $(CV_INC_DIR)/cv_net.h \
			$(CV_INC_DIR)/status/externs.h $(CV_INC_DIR)/status/status.h

globals.o:	globals.c $(CV_INC_DIR)/cv.h $(CV_INC_DIR)/cv_net.h \
			$(CV_INC_DIR)/status/externs.h $(CV_INC_DIR)/status/status.h

clean:
	rm -rf $(OBJS) $(TARGETS)


valgrind:	cv_db
	valgrind --leak-check=full --show-leak-kinds=all --read-var-info=yes -v ./gen_sss -c $(PORT) -s 115200 -D 2 -d DEBUG 2>stderr

