#	autonomous database Makefile


CV_INC_DIR	= ../include
INC		= -I$(CV_INC_DIR)
CFLAGS	+= $(INC) -pthread -g $(DEBUG)

LDFLAGS	+= -pthread

SRCS	= cv_db.c init.c globals.c lists.c utility.c threads.c sensors.c

OBJS	= ${SRCS:.c=.o}

TARGETS	= cv_db



all:	$(TARGETS)



cv_db:	$(OBJS)

cv_db.o:	cv_db.c $(CV_INC_DIR)/cv.h $(CV_INC_DIR)/db/externs.h \
			$(CV_INC_DIR)/db/lists.h

init.o:		init.c $(CV_INC_DIR)/cv.h $(CV_INC_DIR)/cv_net.h \
			$(CV_INC_DIR)/db/externs.h

globals.o:	globals.c $(CV_INC_DIR)/cv.h $(CV_INC_DIR)/cv_net.h

lists.o:	lists.c $(CV_INC_DIR)/cv.h $(CV_INC_DIR)/cv_net.h \
			$(CV_INC_DIR)/db/externs.h $(CV_INC_DIR)/db/lists.h

utility.o:	utility.c $(CV_INC_DIR)/cv.h $(CV_INC_DIR)/db/externs.h

threads.o:	threads.c $(CV_INC_DIR)/cv.h $(CV_INC_DIR)/db/externs.h \
			$(CV_INC_DIR)/sensors.h $(CV_INC_DIR)/db/threads.h \
			$(CV_INC_DIR)/db/lists.h

sensors.o:	sensors.c $(CV_INC_DIR)/cv.h $(CV_INC_DIR)/db/externs.h \
			$(CV_INC_DIR)/sensors.h \
			$(CV_INC_DIR)/db/lists.h



clean:
	rm -rf $(OBJS) $(TARGETS)
