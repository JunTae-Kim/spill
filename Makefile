CC = g++
CFLAGS = -g -Wall
SRCS = test1.cpp
PROG = $(notdir $(CURDIR))

OPENCV = `pkg-config opencv --cflags --libs`
LASPICAM = -lraspicam -lraspicam_cv
LIBS = $(OPENCV) $(LASPICAM)
WIRINGPI = '-lwiringPi'

.PHONY: all clean

$(PROG):$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS) $(WIRINGPI)

all: $(PROG)

clean:
	rm -f $(OBJS) $(PROG)
