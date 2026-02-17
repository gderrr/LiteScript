CXX = g++
CC  = gcc

# -------------------------
# Compiler flags
# -------------------------
CXXFLAGS ?= -O2 -std=c++17
CFLAGS   ?= -O2

# -------------------------
# Linker flags (system libs)
# -------------------------
LDFLAGS  ?= -lglfw -lGL -ldl -lpthread -lpqxx -lpq

# Uncomment to enable AddressSanitizer
#CXXFLAGS += -fsanitize=address
#CFLAGS   += -fsanitize=address
#LDFLAGS  += -fsanitize=address

# -------------------------
# Target
# -------------------------
TARGET = lite

# -------------------------
# Source files
# -------------------------
SRCS_CPP = main.cpp CustomFunctions.cpp Evaluator.cpp Extras.cpp \
           imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_widgets.cpp imgui/imgui_tables.cpp \
           imgui/imgui_impl_glfw.cpp imgui/imgui_impl_opengl3.cpp

SRCS_C   = glad/glad.c

# -------------------------
# Object files
# -------------------------
OBJS_CPP = $(SRCS_CPP:.cpp=.o)
OBJS_C   = $(SRCS_C:.c=.o)
OBJS     = $(OBJS_CPP) $(OBJS_C)

# -------------------------
# Build target
# -------------------------
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)

# Compile C++ sources
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile C sources
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -f $(OBJS) $(TARGET)