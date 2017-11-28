CC		= gcc
CFLAGS		= -g -c -Wall 
LDFLAGS		= -lcrypto
SOURCES		= main.c client.c crypto.c chunk.c ../common/utils.c ../common/tcp.c
INCLUDES	= -I../common/
OBJECTS		= $(SOURCES:.c=.o)
TARGET		= client

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS) 
	$(CC)  $(OBJECTS) $(LDFLAGS) -o $@ 

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

clean:
	rm -rf $(OBJECTS) $(TARGET)

