OBJS	= myport.o functions.o PLedger.o Port-Visual.o VNotes.o queue.o
SOURCE	= myport.c functions.c PLedger.c Port-Visual.c VNotes.c queue.o
HEADER	= functions.h PLedger.h Port-Visual.h VNotes.h queue.h
OUT	= myport
CC	= gcc
FLAGS	= -g -c

all: myport port-master vessel monitor

$(OUT): $(OBJS)
	$(CC) -g $(OBJS) -o $@ -lpthread

port-master: port-master.o functions.o PLedger.o Port-Visual.o VNotes.o queue.o
	$(CC) -g port-master.o functions.o PLedger.o Port-Visual.o VNotes.o queue.o -o $@ -lpthread

vessel: vessel.o functions.o PLedger.o Port-Visual.o VNotes.o queue.o
	$(CC) -g vessel.o functions.o PLedger.o Port-Visual.o VNotes.o queue.o -o $@ -lpthread

monitor: monitor.o functions.o PLedger.o Port-Visual.o VNotes.o queue.o
	$(CC) -g monitor.o functions.o PLedger.o Port-Visual.o VNotes.o queue.o -o $@ -lpthread


#create-compile individual files.
myport.o: myport.c
	$(CC) $(FLAGS) myport.c

functions.o: functions.c
	$(CC) $(FLAGS) functions.c

PLedger.o: PLedger.c
	$(CC) $(FLAGS) PLedger.c

Port-Visual.o: Port-Visual.c
	$(CC) $(FLAGS) Port-Visual.c

port-master.o: port-master.c
	$(CC) $(FLAGS) port-master.c

vessel.o: vessel.c
	$(CC) $(FLAGS) vessel.c

monitor.o: monitor.c
	$(CC) $(FLAGS) monitor.c

VNotes.o: VNotes.c
	$(CC) $(FLAGS) VNotes.c

queue.o: queue.c
	$(CC) $(FLAGS) queue.c


#clean place
clean:
	rm -f $(OBJS) $(OUT) *.log port-master port-master.o vessel vessel.o monitor monitor.o

#acounting
count:
	wc $(SOURCE) $(HEADER) port-master.c vessel.c monitor.c
