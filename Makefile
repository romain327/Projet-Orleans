CC = gcc
CFLAGS = -Wall -g2

SRCFILES = dump_base.c partie_1.c partie_2.c
HDRFILES = partie_1.h partie_2.h
OBJFILES = $(patsubst %.c, %.o, $(SRCFILES))

TARGET = a.out

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c $(HDRFILES)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f $(OBJFILES) $(TARGET)