EXE = bsIRC
CFLAGS = -c -O2 -Wall -Wextra -Werror -I./ -I./libnet
LFLAGS = -lstdc++ -lsyllable -L./libnet -lnet

APPBIN = /Applications/$(EXE)
OBJS = main.o application.o mainwindow.o commthread.o

OBJDIR := objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

# Rules
$(OBJDIR)/%.o : %.c
	@echo Compiling : $<
	@$(CC) $(CFLAGS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	@echo Compiling : $<
	@$(CC) $(CFLAGS) $< -o $@

$(OBJDIR)/%.o : %.s
	@echo Assembling : $<
	@$(CC) $(CFLAGS) -x assembler-with-cpp $< -o $@

all : translations objs
	@cd libnet && $(MAKE)
	@$(MAKE) $(EXE)

translations:

objs:
	@mkdir -p objs

$(EXE): $(OBJS)
	@echo Linking...
	@gcc $(OBJS) -o $(EXE)  $(LFLAGS)
	@echo Adding resources...
	@rescopy $(EXE) -r resources/move-down.png
	@rescopy $(EXE) -a resources/move-up.png
	@rescopy $(EXE) -a resources/remove.png 
	@addattrib $(EXE) os::Category Internet

clean:
	@echo Cleaning...
	@cd libnet && $(MAKE) clean
	@rm -f $(OBJDIR)/*
	@rm -f $(EXE)

deps:

dist: all
	@echo Distribution...
	@mkdir -p $(DIST_DIR)/Applications/$(EXE)
	@cp $(EXE) $(DIST_DIR)/Applications/$(EXE)/$(EXE)

install: all
	@echo Installing...
	@mkdir -p /Applications/$(EXE)
	@cp $(EXE) /Applications/$(EXE)/$(EXE)

