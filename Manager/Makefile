Manager: main.o folderWindow.o filesView.o mimeTable.o
	g++ -o Manager main.o folderWindow.o filesView.o mimeTable.o -latheos

main.o: main.cpp main.h
	g++ -c main.cpp

folderWindow.o: folderWindow.cpp main.h folderWindow.h mimeTable.h
	g++ -c folderWindow.cpp

filesView.o: filesView.cpp main.h filesView.h mimeTable.h
	g++ -c filesView.cpp

mimeTable.o: mimeTable.cpp mimeTable.h
	g++ -c mimeTable.cpp
