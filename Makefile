CXX = g++
CXXFLAGS = -Wall -std=c++11
TARGET = ano_encoder_test
DIAGNOSTIC = test_diagnostic
ENCODER_ONLY = test_encoder_only
I2C_RAW = test_i2c_raw
COMPARE = compare_test
SIMPLE = test_simple
FIXED = ano_encoder_test_fixed
RESET_TEST = test_with_reset
ENCODER_DEBUG = test_encoder_debug

all: $(TARGET) $(DIAGNOSTIC) $(ENCODER_ONLY) $(I2C_RAW) $(COMPARE) $(SIMPLE) $(FIXED) $(RESET_TEST) $(ENCODER_DEBUG)

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

$(SIMPLE): test_simple.cpp ano_encoder_fixed.cpp
	$(CXX) $(CXXFLAGS) -o $(SIMPLE) test_simple.cpp ano_encoder_fixed.cpp

$(FIXED): main.cpp ano_encoder_fixed.cpp
	$(CXX) $(CXXFLAGS) -o $(FIXED) main.cpp ano_encoder_fixed.cpp

$(RESET_TEST): test_with_reset.cpp
	$(CXX) $(CXXFLAGS) -o $(RESET_TEST) test_with_reset.cpp

$(ENCODER_DEBUG): test_encoder_debug.cpp
	$(CXX) $(CXXFLAGS) -o $(ENCODER_DEBUG) test_encoder_debug.cpp

clean:
	rm -f $(TARGET) $(DIAGNOSTIC) $(ENCODER_ONLY) $(I2C_RAW) $(COMPARE) $(SIMPLE) $(FIXED) $(RESET_TEST) $(ENCODER_DEBUG)

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

simple: $(SIMPLE)
	sudo ./$(SIMPLE)

fixed: $(FIXED)
	sudo ./$(FIXED)

reset: $(RESET_TEST)
	sudo ./$(RESET_TEST)

encdebug: $(ENCODER_DEBUG)
	sudo ./$(ENCODER_DEBUG)

