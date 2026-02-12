CXX = g++

CXXFLAGS ?= -O2 -std=c++17
LDFLAGS ?= -lglfw -lGL -ldl -lpthread
ifeq ($(ASan),1)
CXXFLAGS += -fsanitize=address
endif

TARGET = lite
SRCS = main.cpp CustomFunctions.cpp Evaluator.cpp Extras.cpp \
       imgui/imgui.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp imgui/imgui_draw.cpp \
	   imgui/imgui_impl_glfw.cpp imgui/imgui_impl_opengl3.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS) $(TARGET)