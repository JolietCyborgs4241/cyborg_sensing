#	vision processing makefile


CV_INC_DIR	= ../include
INC		= -I$(CV_INC_DIR)
CFLAGS	+= $(INC) -pthread -g # -DDEBUG 

LDFLAGS	+= -pthread

SRCS	= cv_proc.c init.c globals.c lists.c utility.c threads.c

OBJS	= ${SRCS:.c=.o}

TARGETS	= cv_proc



all:	$(TARGETS)



cv_proc:	$(OBJS)

cv_proc.o:	cv_proc.c $(CV_INC_DIR)/cv.h $(CV_INC_DIR)/cv_externs.h

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
