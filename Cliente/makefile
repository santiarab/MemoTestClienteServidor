CXX = g++
CXXFLAGS = -Iheader -Wall
SRCS = src/main.cpp src/funciones.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = Cliente

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)