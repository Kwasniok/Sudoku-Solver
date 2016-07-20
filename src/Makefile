#O2 for optimization, g for debugging
CFLAGS=-Wall -O2 -g --std=c++14 #-I. $(shell root-config --cflags)
LFLAGS= #$(shell root-config --libs)
CC=g++
LD=g++

#all source files
SRCS=$(wildcard *.cpp)

OBJS = $(filter-out main.o, $(SRCS:.cpp=.o))

.PHONY: clean all

all: main

clean:
	@rm -f *~ *.o *# *.d

main: $(OBJS) main.o
	$(LD) $(LFLAGS) -o main $^


#rules
%.o : %.cpp
	$(CC) $(CFLAGS) -MMD -c -o $@ $<
	@sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
             -e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.d

-include $(SRCS:%.cpp=%.d)

