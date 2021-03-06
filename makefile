CC = g++
CFLAGS = -g -Wall
SRCS = morphology.cpp
PROG = $(notdir $(CURDIR))

OPENCV = `pkg-config opencv --cflags --libs`
LASPICAM = -lraspicam -lraspicam_cv
LIBS = $(OPENCV) $(LASPICAM)
WIRINGPI = '-lwiringPi'
LOCATION = '/home/pi/wiringPi/wiringPi/softServo.c'
.PHONY: all cleans

$(PROG):$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS) $(LOCATION) $(WIRINGPI)

all: $(PROG)

clean:
	rm -f $(OBJS) $(PROG)
