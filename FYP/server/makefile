CC		= gcc
CFLAGS		= -g -c -Wall
LDFLAGS		= -pthread -lcrypto
SOURCES		= main.c server.c ../common/tcp.c dedup.c fs.c ../common/utils.c ../common/debug.c
INCLUDES	= -I../common/
OBJECTS		= $(SOURCES:.c=.o)
TARGET		= server

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS) 
	$(CC)  $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

clean:
	rm -rf $(OBJECTS) $(TARGET)
	rm -rf data/*
