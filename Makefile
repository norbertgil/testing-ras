CXX = g++
CXXFLAGS = -Wall -std=c++11
TARGET = ano_encoder_test
DIAGNOSTIC = test_diagnostic

all: $(TARGET) $(DIAGNOSTIC)

$(TARGET): main.cpp ano_encoder.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.cpp ano_encoder.cpp

$(DIAGNOSTIC): test_diagnostic.cpp ano_encoder.cpp
	$(CXX) $(CXXFLAGS) -o $(DIAGNOSTIC) test_diagnostic.cpp ano_encoder.cpp

clean:
	rm -f $(TARGET) $(DIAGNOSTIC)

run: $(TARGET)
	sudo ./$(TARGET)

diagnostic: $(DIAGNOSTIC)
	sudo ./$(DIAGNOSTIC)

