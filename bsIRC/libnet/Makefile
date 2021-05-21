COPTS = -Wall -c -O2 -pipe -fexceptions
LIB = libnet.a

OBJS = ipaddress.o host.o url.o socket.o datastream.o httpstream.o datafactory.o

# Rules
%.o: %.c
	@echo Compiling : $<
	@$(CC) $(COPTS) $< -o $@

%.o: %.cpp
	@echo Compiling : $<
	@$(CXX) $(COPTS) $< -o $@

%.o: %.s
	@echo Assembling : $<
	@$(CC) $(COPTS) -x assembler-with-cpp $< -o $@

all: $(OBJS)
	@echo Linking...
	@ar rcv $(LIB) $(OBJS)
	@ranlib $(LIB)

clean:
	@echo Cleaning...
	@-rm -f $(LIB) $(OBJS)



