


out/appiso.exe:  main/src/appiso.cpp boot/src/boot.cpp  formats/joliet/src/joliet.cpp
	g++ main/src/appiso.cpp -o $(@)  -std=c++11 -O2 -m32
# lib/joliet.o: formats/joliet/src/joliet.cpp formats/joliet/inc/joliet.h
# 	g++ formats/joliet/src/joliet.cpp -c -o $(@) -std=c++11
clean:
	del lib/*.o