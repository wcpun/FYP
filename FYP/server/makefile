CC		= gcc
CFLAGS		= -g -c -Wall
LDFLAGS		= -pthread -lcrypto
SOURCES		= main.c server.c ../common/tcp.c dedup.c ../common/utils.c
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

