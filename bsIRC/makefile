#Makefile for bsIRC
COPTS = -Wall -c -O2 -fexceptions -ffast-math

APPBIN = /Applications/bsIRC
OBJS = main.o application.o mainwindow.o postoffice1.o

OBJDIR := objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

# Rules
$(OBJDIR)/%.o : %.c
	@echo Compiling : $<
	@$(CC) $(COPTS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	@echo Compiling : $<
	@$(CC) $(COPTS) $< -o $@

$(OBJDIR)/%.o : %.s
	@echo Assembling : $<
	@$(CC) $(COPTS) -x assembler-with-cpp $< -o $@

all : translations objs "bsIRC"

$(OBJDIR)/mainwindow.o: mainwindowLayout.cpp

translations:

objs:
	@mkdir -p objs

"bsIRC": $(OBJS)
	@echo Linking...
	@gcc $(OBJS) -o "bsIRC"  -lstdc++ -lsyllable
	@echo Adding resources...
	@rescopy "bsIRC" -r resources/move-down.png resources/move-up.png resources/remove.png 
	@addattrib "bsIRC" os::Category Internet

clean:
	@echo Cleaning...
	@rm -f $(OBJDIR)/*
	@rm -f "bsIRC"

deps:

dist: all
	@echo Distribution...
	@mkdir -p $(DIST_DIR)/Applications/bsIRC
	@cp "bsIRC" $(DIST_DIR)/Applications/bsIRC/"bsIRC"

install: all
	@echo Installing...
	@mkdir -p /Applications/bsIRC
	@cp "bsIRC" /Applications/bsIRC/"bsIRC"

