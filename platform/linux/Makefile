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
#CFLAGS += -I../../src/lib/wxWidgets/gtk-build/lib/wx/include/gtk3-unicode-3.1

###################################################

OBJDIR=obj
BINDIR=bin
MAIN_SRCS = main.cpp

#LIB_SRCS = <TODO: add files if needed>

EMIDI_LIB_SRCS = midifile.c midiplayer.c helpers.c emidi_linux.c

PROJ_NAME=FloppyMusicDAW
OUTPATH=bin

###################################################

MAIN_OBJS=$(patsubst %.cpp,$(OBJDIR)/%.o,$(MAIN_SRCS))
#LIB_OBJS=$(patsubst %.c,$(OBJDIR)/%.o,$(LIB_SRCS))
EMIDI_LIB_OBJS=$(patsubst %.c,$(OBJDIR)/%.o,$(EMIDI_LIB_SRCS))

###################################################

.PHONY: proj clean

all: $(OUTPATH)/$(PROJ_NAME).elf

$(OBJDIR):
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@ -L$(OBJDIR)

$(OBJDIR)/libemidi.a: $(EMIDI_LIB_OBJS)
	$(AR) -r $@ $(EMIDI_LIB_OBJS)

$(OBJDIR)/main.o: main.cpp
	$(CXX) $(CFLAGS) -c $< -o $@ -L$(OBJDIR) `wx-config --cxxflags --libs`

$(OUTPATH)/$(PROJ_NAME).elf: $(OBJDIR)/libemidi.a $(MAIN_OBJS) $(OBJDIR)/main.o
	$(CXX) $(CFLAGS) $^ -o $@ -L. -Lobj -lemidi `wx-config --cxxflags --libs`
	$(SIZE) $@

proj: $(OUTPATH)/$(PROJ_NAME).elf

clean:
	rm -rf $(OBJDIR)
	rm -rf $(BINDIR)

