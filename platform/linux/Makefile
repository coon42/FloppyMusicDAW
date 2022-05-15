BINPATH=/usr/bin

CC=$(BINPATH)/gcc
CXX=$(BINPATH)/g++
AR=$(BINPATH)/ar
LD=$(BINPATH)/ld
OBJCOPY=$(BINPATH)/objcopy
SIZE=$(BINPATH)/size

vpath %.cpp ../../src
vpath %.c ../../src/lib/eMIDI/src
vpath %.c ../../src/lib/eMIDI/src/hal

CFLAGS  = -g -Wall -pthread -m64 # TODO: use -m32
CFLAGS += -I../../src
CFLAGS += -I../../src/lib/eMIDI/src
CFLAGS += -I../../src/lib/eMIDI/src/hal
CFLAGS += -I../../src/lib/wxWidgets/include

###################################################

MAIN_SRCS = song.cpp keyeditor.cpp trackeditor.cpp transport.cpp main.cpp
MAIN_OBJS=$(patsubst %.cpp,obj/%.o,$(MAIN_SRCS))

PROJ_NAME=FloppyMusicDAW

.PHONY: proj clean

all: bin/$(PROJ_NAME).elf

obj:
	mkdir -p obj
	mkdir -p bin

obj/%.o: %.c | obj
	$(CC) $(CFLAGS) -c $< -o $@ -Lobj

obj/%.o: %.cpp | obj
	$(CXX) $(CFLAGS) -c $< -o $@ -Lobj `wx-config --cxxflags --libs`

../../src/lib/eMIDI/lib/libemidi.a:
	$(MAKE) lib/libemidi.a -C ../../src/lib/eMIDI

bin/$(PROJ_NAME).elf: ../../src/lib/eMIDI/lib/libemidi.a $(MAIN_OBJS)
	$(CXX) $(CFLAGS) $(MAIN_OBJS) -o $@ -L ../../src/lib/eMIDI/lib -lemidi `wx-config --cxxflags --libs`
	$(SIZE) $@

proj: bin/$(PROJ_NAME).elf

clean:
	rm -rf obj
	rm -rf bin

