# name of C compiler and flags for C compiler
CC       = cc
CXX  	 = CC
CFLAGS   = -mips2 -o32 -I/usr/local/include/GL
CXXFLAGS = -g -fullwarn -mips2 -o32 -I/usr/local/include/GL -I/Net/laika/Users/cecs361/fltk


# OpenGL libraries
GL_LIBS  = -lGL -lGLU

# libraries used by glut_dda executable
GLUT_LIB = /usr/local/lib/libglut.a $(GL_LIBS) -lX11 -lXmu -lm

all: pong

PONG_OBJS = pong.o pFunk.o

pong: $(PONG_OBJS) pong3d.h
	$(CC) $(CFLAGS) -o pong $(PONG_OBJS) $(GLUT_LIB)

clean:
	rm -f *.o;
