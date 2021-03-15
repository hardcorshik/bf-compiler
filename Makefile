CC = g++
CFLAGS = -g -Wall
TARGET = bfc
INSTALL = install
INSTALL_LOCATION = /usr/bin

all: $(TARGET)

$(TARGET): $(TARGET).cpp
	$(CC) $(CFLAGS) $(TARGET).cpp -o $(TARGET)

clean:
	rm $(TARGET)

install: all
	$(INSTALL) -D $(TARGET) $(INSTALL_LOCATION)