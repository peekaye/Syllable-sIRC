#Makefile for SBasic
COPTS = -c -fexceptions -ffast-math

APPBIN = /system/bin
OBJS = sIRC32.o postoffice1.o


OBJDIR := objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

# Rules
$(OBJDIR)/%.o : %.c
	@echo Compiling : $<
	@$(CC) $(COPTS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	@echo Compiling : $<
	@$(CXX) $(COPTS) $< -o $@

$(OBJDIR)/%.o : %.s
	@echo Assembling : $<
	@$(CC) $(COPTS) -x assembler-with-cpp $< -o $@

all : objs sIRC

objs:
	@mkdir -p objs

sIRC: $(OBJS)
	@echo Linking...
	@$(CXX) $(OBJS) -o sIRC -lsyllable

clean:
	@echo Cleaning...
	@rm -f $(OBJDIR)/*
	@rm -f sIRC

