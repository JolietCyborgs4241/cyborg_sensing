#	status server Makefile


CV_INC_DIR	= ../../include
INC		= -I$(CV_INC_DIR)
CFLAGS	+= $(INC) -pthread -g $(DEBUG)

CY_LIBS = -L ../../lib -lcyborgauto
LDFLAGS	+= -pthread $(CY_LIBS)

SRCS	= cyborg_stats.c init.c globals.c

OBJS	= ${SRCS:.c=.o}

TARGETS	= cyborg_stats


all:	$(TARGETS)


cyborg_stats:	$(OBJS)

cyborg_stats.o:	cyborg_stats.c $(CV_INC_DIR)/cv.h $(CV_INC_DIR)/cv_net.h \
			$(CV_INC_DIR)/gen_sss/externs.h

init.o:		init.c $(CV_INC_DIR)/cv.h $(CV_INC_DIR)/cv_net.h \
			$(CV_INC_DIR)/gen_sss/externs.h

clean:
	rm -rf $(OBJS) $(TARGETS)


valgrind:	cv_db
	valgrind --leak-check=full --show-leak-kinds=all --read-var-info=yes -v ./gen_sss -c $(PORT) -s 115200 -D 2 -d DEBUG 2>stderr

