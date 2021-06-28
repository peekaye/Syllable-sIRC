Manager: main.o folderWindow.o filesView.o mimeTable.o directoryview.o
	g++ -o Manager main.o folderWindow.o filesView.o mimeTable.o directoryview.o -lsyllable

main.o: main.cpp main.h
	g++ -c -Wall main.cpp

folderWindow.o: folderWindow.cpp main.h folderWindow.h mimeTable.h
	g++ -c -Wall folderWindow.cpp

filesView.o: filesView.cpp main.h filesView.h mimeTable.h
	g++ -c -Wall filesView.cpp

mimeTable.o: mimeTable.cpp mimeTable.h
	g++ -c -Wall mimeTable.cpp

directoryview.o: directoryview.cpp
	g++ -c -Wall directoryview.cpp

clean:
	rm -f main.o folderWindow.o filesView.o mimeTable.o directoryview.o
	rm -f Manager
