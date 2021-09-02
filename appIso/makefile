


out/appiso.exe: lib/joliet.o main/src/appiso.cpp  formats/joliet/src/joliet.cpp
	g++ main/src/appiso.cpp -o $(@) lib/joliet.o  -std=c++11
lib/joliet.o: formats/joliet/src/joliet.cpp formats/joliet/inc/joliet.h
	g++ formats/joliet/src/joliet.cpp -c -o $(@) -std=c++11
clean:
	del lib/*.o