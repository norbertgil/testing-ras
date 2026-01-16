CXX = g++
CXXFLAGS = -Wall -std=c++11
TARGET = encoder_app

all: $(TARGET)

$(TARGET): encoder_app.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) encoder_app.cpp

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
