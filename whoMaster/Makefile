bin/test: source/main.cpp lib/mainAux.o
	g++ -std=c++11 -ggdb3 -o master source/main.cpp lib/mainAux.o ../auxiliarry/auxiliarry.o
	mv master ../

lib/mainAux.o: source/mainAux.cpp headers/mainAux.h ../auxiliarry/auxiliarry.o
	g++ -std=c++11 -ggdb3 -c source/mainAux.cpp
	mv mainAux.o lib/mainAux.o

clean:
	rm lib/*
