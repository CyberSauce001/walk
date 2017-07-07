CFLAGS = -I ./include
LFLAGS = -lrt -lX11 -lGLU -lGL -pthread -lm #-lXrandr

all: walk

walk: walk.cpp ppm.cpp log.cpp
	g++ $(CFLAGS) walk.cpp lab3http.cpp ppm.cpp log.cpp libggfonts.a -Wall -Wextra  $(LFLAGS) -owalk -D UNIT_TEST -D ALEX_UNIT_TEST

clean:
	rm -f walk
	rm -f *.o

