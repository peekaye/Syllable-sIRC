CC=g++
CFLAGS=-Wall
LFLAGS=-Lexpat
LIBS=-lsyllable -lexpat
TARGET=Chat
VERSION=0.0.9



OBJ=objs/app.o objs/configfile.o objs/jabber.o objs/poller.o objs/socket.o objs/wchat.o objs/wlog.o objs/woptions.o objs/wroster.o

$(TARGET): $(OBJ)
	$(CC) $(LFLAGS) $(OBJ) -o $(TARGET) $(LIBS)

clean:
	rm -f $(OBJ) $(TARGET)

bk:
	mkdir -p backup
	rm -f backup/chat`date +%y%m%d`.tar.gz
	tar czf backup/chat`date +%y%m%d`.tar.gz *.cpp *.h config TODO COPYING COPYING.expat

install: $(TARGET)
	mkdir -p /Applications/Chat
	cp -f $(TARGET) /Applications/$(TARGET)/$(TARGET)
	ln -s /Applications/$(TARGET)/$(TARGET) /atheos/autolnk/bin/$(TARGET)

rel: $(TARGET)
	mkdir -p rel
	mkdir chat-src_$(VERSION)
	cp -p -P *.cpp *.h Makefile COPYING expat/* chat-src_$(VERSION)
	mkdir chat-src_$(VERSION)/objs
	tar czf rel/chat-src_$(VERSION).tar.gz chat-src_$(VERSION)
	rm -r chat-src_$(VERSION)
	mkdir chat_$(VERSION)
	cp -p -P Chat COPYING COPYING.expat chat_$(VERSION)
	echo -e '#!/bin/sh\n\nmkdir -p /Applications/Chat\ncp -f Chat /Applications/Chat\nln -s /Applications/Chat/Chat /atheos/autolnk/bin/Chat\n' > chat_$(VERSION)/install.sh
	chmod 755 chat_$(VERSION)/install.sh
	tar czf rel/chat_$(VERSION).tar.gz chat_$(VERSION)
	rm -r ON)




objs/app.o: app.cpp expat/expat.h app.h wroster.h wlog.h socket.h poller.h error.h def.h configfile.h jabber.h
	$(CC) $(CFLAGS) -c app.cpp -o objs/app.o

objs/configfile.o: configfile.cpp configfile.h
	$(CC) $(CFLAGS) -c configfile.cpp -o objs/configfile.o

objs/jabber.o: jabber.cpp jabber.h app.h def.h expat/expat.h configfile.h wlog.h
	$(CC) $(CFLAGS) -c jabber.cpp -o objs/jabber.o

objs/poller.o: poller.cpp poller.h socket.h error.h
	$(CC) $(CFLAGS) -c poller.cpp -o objs/poller.o

objs/socket.o: socket.cpp socket.h error.h
	$(CC) $(CFLAGS) -c socket.cpp -o objs/socket.o

objs/wchat.o: wchat.cpp wchat.h wroster.h app.h expat/expat.h configfile.h jabber.h wlog.h
	$(CC) $(CFLAGS) -c wchat.cpp -o objs/wchat.o

objs/wlog.o: wlog.cpp wlog.h
	$(CC) $(CFLAGS) -c wlog.cpp -o objs/wlog.o

objs/woptions.o: woptions.cpp woptions.h wroster.h app.h configfile.h def.h expat/expat.h jabber.h wlog.h
	$(CC) $(CFLAGS) -c woptions.cpp -o objs/woptions.o

objs/wroster.o: wroster.cpp wroster.h woptions.h wchat.h wlog.h app.h def.h expat/expat.h configfile.h jabber.h
	$(CC) $(CFLAGS) -c wroster.cpp -o objs/wroster.o

