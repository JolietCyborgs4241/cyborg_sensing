#	vision processing makefile


CV_INC_DIR	= ../include
INC		= -I$(CV_INC_DIR)
CFLAGS	+= $(INC)

LDFLAGS	+= 

SRCS	= cv_proc.c init.c globals.c
OBJS	= ${SRCS:.c=.o}

TARGETS	= cv_proc



all:	$(TARGETS)



cv_proc:	$(OBJS)

cv_proc.o:	cv_proc.c $(CV_INC_DIR)/cv.h $(CV_INC_DIR)/cv_externs.h

init.o:		init.c $(CV_INC_DIR)/cv.h $(CV_INC_DIR)/cv_net.h $(CV_INC_DIR)/cv_externs.h

globals.o:	globals.c $(CV_INC_DIR)/cv.h $(CV_INC_DIR)/cv_net.h $(CV_INC_DIR)/cv_cam.h $(CV_INC_DIR)/cv_externs.h



clean:
	rm -rf $(OBJS) $(TARGETS)
