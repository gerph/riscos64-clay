##
# Build for the Clay example
#

CC = riscos64-cc
LD = riscos64-link
CFLAGS = -Igcontext/ -O2

GCONTEXT_OBJS = \
		gcontext/border.o \
		gcontext/font.o \
		gcontext/gcontext.o \
		gcontext/screen.o \
		gcontext/vdu5.o \
		gcontext/vdushape.o \

APP_OBJS = riscos.o

all: example,ff8

gcontext/gcontext.a: ${GCONTEXT_OBJS}
	riscos64-libfile -c $@ ${GCONTEXT_OBJS}

example,ff8: gcontext/gcontext.a ${APP_OBJS}
	riscos64-link -o $@ ${APP_OBJS} gcontext/gcontext.a
