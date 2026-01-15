CXX = g++
CXXFLAGS = -Wall -std=c++11
TARGET = ano_encoder_test

all: $(TARGET)

$(TARGET): main.cpp ano_encoder.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.cpp ano_encoder.cpp

clean:
	rm -f $(TARGET)

run: $(TARGET)
	sudo ./$(TARGET)

