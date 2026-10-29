CXX ?= g++
CXXFLAGS ?= -std=c++11 -O2 -Wall -Wextra
TARGET = ascii_game

all: $(TARGET)

$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.cpp

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run