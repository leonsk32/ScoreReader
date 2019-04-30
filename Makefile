CC = g++
CFLAGS	= 
LDFLAGS	=
INCLUDES  = ${shell pkg-config opencv --cflags}
LIBS += ${shell pkg-config opencv --libs}

TARGET1 = score_reader
OBJS1 = score_reader.o staff.o labeling.o distinction.o

all:	$(TARGET1)

$(TARGET1): $(OBJS1)
	$(CC) $(LDFLAGS) -o $@ $(OBJS1) $(LIBS)

staff.o: staff.h
labeling.o: labeling.h
distinction.o: distinction.h

clean:
	-rm -f $(TARGET1) $(OBJS1) *.o *~

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

