EXE = sIRC
CFLAGS = -c -O2 -Wall -Wextra
LFLAGS = -lstdc++ -lsyllable

APPBIN = /system/bin
OBJS = sIRC32.o commthread.o


OBJDIR := objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

# Rules
$(OBJDIR)/%.o : %.c
	@echo Compiling : $<
	@$(CC) $(CFLAGS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	@echo Compiling : $<
	@$(CXX) $(CFLAGS) $< -o $@

$(OBJDIR)/%.o : %.s
	@echo Assembling : $<
	@$(CC) $(CFLAGS) -x assembler-with-cpp $< -o $@

all : objs $(EXE)

objs:
	@mkdir -p objs

$(EXE): $(OBJS)
	@echo Linking...
	@$(CXX) $(OBJS) -o $(EXE) $(LFLAGS)

clean:
	@echo Cleaning...
	@rm -f $(OBJDIR)/*
	@rm -f $(EXE)

