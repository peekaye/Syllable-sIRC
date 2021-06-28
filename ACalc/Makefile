COPTS	= -Wall -c

OBJS	= main.o gui/colorbutton.o gui/textview2.o
EXE	= acalc

CC	= gcc
LL	= gcc

$(EXE) : $(OBJS)
	$(LL) $(OBJS) -lsyllable -lstdc++ -lm -o $@

install: $(EXE)
	cp -f $(EXE) /system/bin/$(EXE)
	cp lcd2n.ttf /system/fonts/
	strip --strip-all /system/bin/$(EXE)
upgrade: $(EXE)
	rfs $(EXE) /system/bin/$(EXE)
clean:
	-rm $(OBJS) $(EXE)
#.c.o :
#	$(CC) $(COPTS) $< -o $@
.cpp.o:
	$(CC) $(COPTS) $< -o $@

main.o:		main.cpp myapp.h mywindow.h


