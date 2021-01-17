#
# In order to execute this "Makefile" just type "make"
#

OBJS 	= OS1_2017.o
SOURCE	= OS1_2017.c
HEADER  = myHeader.h
OUT  	= OS1_2017
CC	= gcc
FLAGS   = -g -c 
# -g option enables debugging mode 
# -c flag generates object code for separate files

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT)

# create/compile the individual files >>separately<< 
OS1_2017.o: OS1_2017.c
	$(CC) $(FLAGS) OS1_2017.c

# clean house
clean:
	rm -f $(OBJS) $(OUT)

# do a bit of accounting
count:
	wc $(SOURCE) $(HEADER)
