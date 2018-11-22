#
# Makefile:
#
# Compiler and linker
CC			:= g++

#The Target Binary Program
TARGET		:= morsamdesa
SRCZIP		:= $(TARGET).zip

SRCDIR		:= src
INCDIR		:= src
BUILDDIR	:= obj
TARGETDIR	:= bin
LOGDIR		:= log

SRCEXT		:= cpp
OBJEXT		:= o

DEBUG		:= -O3

INC		:= -I$(INCDIR) -I/usr/include -I/usr/include/libxml2 -I/usr/include/lame
CFLAGS	:= $(DEBUG) -Wall $(INCLUDE) -pipe
LIB		:= -L/usr/local/lib -lpulse-simple -lpulse -lcurl -lconfig++ -lmp3lame -L/usr/lib -lxml2 -lpthread -lm

SOURCES	 := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS	 := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))

# Make the directories
directories:
	@mkdir -p $(TARGETDIR)
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(LOGDIR)

# Clean only object files
clean:
	@$(RM) -rf $(BUILDDIR)

# Link
$(TARGET):	$(OBJECTS)
	@echo [link]
	$(CC) -o $(TARGETDIR)/$(TARGET) $^ $(LIB)
	@rm $(SRCZIP)

# Compile
$(BUILDDIR)/%.$(OBJEXT):	$(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

all		:	$(TARGET)
