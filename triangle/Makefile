CC		= g++
CFLAGS		= -std=c++11 -Wall -march=native `pkg-config --cflags glfw3 glew`
LINKFLAGS	= `pkg-config --libs glfw3 glew`
SRCS		= main.cpp ../common/util.cpp
OBJS		= $(SRCS:.cpp=.o)
PROG		= main

all: $(SRCS) $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(INCFLAGS) $(LINKFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -c -o $@ $(INCFLAGS)

clean:
	rm $(OBJS) $(PROG)
