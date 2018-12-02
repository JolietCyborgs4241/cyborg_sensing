#	autonomous database Makefile


CV_INC_DIR	= ../include
INC		= -I$(CV_INC_DIR)
CFLAGS	+= $(INC) -pthread -g #-DDEBUG 

LDFLAGS	+= -pthread

SRCS	= cv_db.c init.c globals.c lists.c utility.c threads.c

OBJS	= ${SRCS:.c=.o}

TARGETS	= cv_db



all:	$(TARGETS)



cv_db:	$(OBJS)

cv_db.o:	cv_db.c $(CV_INC_DIR)/cv.h $(CV_INC_DIR)/cv_externs.h

init.o:		init.c $(CV_INC_DIR)/cv.h $(CV_INC_DIR)/cv_net.h \
			$(CV_INC_DIR)/cv_externs.h

globals.o:	globals.c $(CV_INC_DIR)/cv.h $(CV_INC_DIR)/cv_net.h \
			$(CV_INC_DIR)/cv_cam.h $(CV_INC_DIR)/cv_externs.h

lists.o:	lists.c $(CV_INC_DIR)/cv.h $(CV_INC_DIR)/cv_net.h \
			$(CV_INC_DIR)/cv_cam.h $(CV_INC_DIR)/cv_externs.h \
			$(CV_INC_DIR)/cv_lists.h

utility.o:	utility.c $(CV_INC_DIR)/cv.h $(CV_INC_DIR)/cv_externs.h

threads.o:	threads.c $(CV_INC_DIR)/cv.h $(CV_INC_DIR)/cv_externs.h \
			$(CV_INC_DIR)/cv_threads.h



clean:
	rm -rf $(OBJS) $(TARGETS)
