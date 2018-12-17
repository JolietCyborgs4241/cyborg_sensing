#	autonomous generic sensor server Makefile


CV_INC_DIR	= ../../include
INC		= -I$(CV_INC_DIR)
CFLAGS	+= $(INC) -pthread -g $(DEBUG)

LDFLAGS	+= -pthread

SRCS	= gen_sss.c init.c globals.c threads.c utility.c

OBJS	= ${SRCS:.c=.o}

TARGETS	= gen_sss


all:	$(TARGETS)


gen_sss:	$(OBJS)

gen_sss.o:	gen_sss.c $(CV_INC_DIR)/cv.h $(CV_INC_DIR)/cv_net.h \
			$(CV_INC_DIR)/gen_sss/externs.h

init.o:		init.c $(CV_INC_DIR)/cv.h $(CV_INC_DIR)/cv_net.h \
			$(CV_INC_DIR)/gen_sss/externs.h

threads.o:	threads.c $(CV_INC_DIR)/cv.h $(CV_INC_DIR)/cv_net.h \
			$(CV_INC_DIR)/gen_sss/externs.h

utility.o:	utility.c $(CV_INC_DIR)/cv.h $(CV_INC_DIR)/cv_net.h \
			$(CV_INC_DIR)/gen_sss/externs.h

clean:
	rm -rf $(OBJS) $(TARGETS)


valgrind:	cv_db
	valgrind --leak-check=full --show-leak-kinds=all --read-var-info=yes -v ./gen_sss -c $(PORT) -s 115200 -D 2 -d DEBUG 2>stderr

