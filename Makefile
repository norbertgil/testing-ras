CXX = g++
CXXFLAGS = -Wall -std=c++11
TARGET = ano_encoder_test
DIAGNOSTIC = test_diagnostic
ENCODER_ONLY = test_encoder_only
I2C_RAW = test_i2c_raw
COMPARE = compare_test

all: $(TARGET) $(DIAGNOSTIC) $(ENCODER_ONLY) $(I2C_RAW) $(COMPARE)

$(TARGET): main.cpp ano_encoder.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.cpp ano_encoder.cpp

$(DIAGNOSTIC): test_diagnostic.cpp ano_encoder.cpp
	$(CXX) $(CXXFLAGS) -o $(DIAGNOSTIC) test_diagnostic.cpp ano_encoder.cpp

$(ENCODER_ONLY): test_encoder_only.cpp ano_encoder.cpp
	$(CXX) $(CXXFLAGS) -o $(ENCODER_ONLY) test_encoder_only.cpp ano_encoder.cpp

$(I2C_RAW): test_i2c_raw.cpp
	$(CXX) $(CXXFLAGS) -o $(I2C_RAW) test_i2c_raw.cpp

$(COMPARE): compare_test.cpp ano_encoder.cpp
	$(CXX) $(CXXFLAGS) -o $(COMPARE) compare_test.cpp ano_encoder.cpp

clean:
	rm -f $(TARGET) $(DIAGNOSTIC) $(ENCODER_ONLY) $(I2C_RAW) $(COMPARE)

run: $(TARGET)
	sudo ./$(TARGET)

diagnostic: $(DIAGNOSTIC)
	sudo ./$(DIAGNOSTIC)

encoder: $(ENCODER_ONLY)
	sudo ./$(ENCODER_ONLY)

rawtest: $(I2C_RAW)
	sudo ./$(I2C_RAW)

compare: $(COMPARE)
	sudo ./$(COMPARE)

