#
# http://www.gnu.org/software/make/manual/make.html
#
CC:=gcc
INCLUDES:=$(shell pkg-config --cflags sdl)

FFMPEG_PATH=/home/frank/forbuild/dist/ffmpeg-4.2.2
# libavformat libavcodec libswresample libswscale
INCLUDES += -I${FFMPEG_PATH}/include 

CFLAGS:=-Wall -ggdb

LDFLAGS:=$(shell pkg-config --libs sdl) -lm -lpthread -lz

# libavformat libavcodec libswresample libswscale libavutil
EXTLIBS += ${FFMPEG_PATH}/lib/libavformat.a
EXTLIBS += ${FFMPEG_PATH}/lib/libavcodec.a 
EXTLIBS += ${FFMPEG_PATH}/lib/libswresample.a 
EXTLIBS += ${FFMPEG_PATH}/lib/libswscale.a 
EXTLIBS += ${FFMPEG_PATH}/lib/libavutil.a


EXE:=tutorial01.out tutorial02.out tutorial03.out tutorial04.out\
	tutorial05.out tutorial06.out tutorial07.out tutorial08.out tutorial09.out 

#
# This is here to prevent Make from deleting secondary files.
#
.SECONDARY:
	

#
# $< is the first dependency in the dependency list
# $@ is the target name
#
all: dirs $(addprefix bin/, $(EXE)) 

dirs:
	mkdir -p obj
	mkdir -p bin

bin/%.out: obj/%.o
	$(CC) $(CFLAGS) $< ${EXTLIBS} $(LDFLAGS) -o $@

obj/%.o : %.c
	$(CC) $(CFLAGS) $< $(INCLUDES) -c -o $@

clean:
	rm -f obj/*
	rm -f bin/*

