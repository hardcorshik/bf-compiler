CC = g++
CFLAGS = -g -Wall
TARGET = bf

all: $(TARGET)

$(TARGET): $(TARGET).cpp
	$(CC) $(CFLAGS) $(TARGET).cpp -o $(TARGET)

clean:
	rm $(TARGET)