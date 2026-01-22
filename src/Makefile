CXX = g++
CXXFLAGS = -O2 -std=c++17
TARGET = lite
SRCS = main.cpp CustomFunctions.cpp Evaluator.cpp Extras.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS) $(TARGET)