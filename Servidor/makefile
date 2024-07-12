CXX = g++
CXXFLAGS = -Iheader -Wall
LDFLAGS = -lrt
SRCS = src/main.cpp src/funciones.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = Servidor

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)