CC = g++
CFLAGS = -g -Wall
SRCS = cam.cpp
PROG = $(notdir $(CURDIR))

OPENCV = `pkg-config opencv --cflags --libs`
LASPICAM = -lraspicam -lraspicam_cv
LIBS = $(OPENCV) $(LASPICAM)
.PHONY: all clean

$(PROG):$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)

all: $(PROG)

clean:
	rm -f $(OBJS) $(PROG)
