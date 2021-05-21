COPTS = -Wall -c --no-warn

OBJS = ../objs/main.o ../objs/application.o ../objs/address.o ../objs/imageitem.o ../objs/loadbitmap.o
EXE	= ../objs/address

CC	= g++
LL	= g++

$(EXE) : $(OBJS)
	@echo Building : $(EXE)
	@$(LL) $(OBJS) -lsyllable -o $(EXE)
	@echo Adding  images to $(EXE)
	@rescopy -r ../objs/address  ./images/*.png


clean:
	@echo Removing all objects and executables 
	@rm -f $(OBJS)
	@rm -f $(EXE)


dist:
	@echo Copying $(EXE) to $(DIST_DIR)/atheos/sys/bin
	@cp  -f $(EXE) $(DIST_DIR)/atheos/sys/bin


astyle:
	@echo Cleaning up all of the source
	@astyle --style=ansi *.cpp *.h
	@echo Removing AStyle backup files
	@rm -rf *.orig

../objs/main.o : main.cpp
	@echo Compiling : $<
	@$(CC) $(COPTS) $< -o $@

../objs/application.o : application.cpp
	@echo Compiling : $<
	@$(CC) $(COPTS) $< -o $@

../objs/address.o : address.cpp
	@echo Compiling : $<
	@$(CC) $(COPTS) $< -o $@

../objs/imageitem.o : imageitem.cpp
	@echo Compiling : $<
	@$(CC) $(COPTS) $< -o $@

../objs/loadbitmap.o : loadbitmap.cpp
	@echo Compiling : $<
	@$(CC) $(COPTS) $< -o $@























