# Project: search
# Makefile created by Dev-C++ 5.11

CPP      = g++
OBJ      = anqi.o ClientSocket.o main.o Protocol.o
LINKOBJ  = anqi.o ClientSocket.o main.o Protocol.o
LIBS     = #-L
INCS     = #-I
CXXINCS  = #-I
BIN      = search
CXXFLAGS = $(CXXINCS) -static -s -std=gnu++11 -g3
CFLAGS   = $(INCS) -std=gnu++11 -g3
RM       = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after

clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o $(BIN) $(LIBS)

anqi.o: anqi.cc
	$(CPP) -c anqi.cc -o anqi.o $(CXXFLAGS)

ClientSocket.o: ClientSocket.cpp
	$(CPP) -c ClientSocket.cpp -o ClientSocket.o $(CXXFLAGS)

main.o: main.cc
	$(CPP) -c main.cc -o main.o $(CXXFLAGS)

Protocol.o: Protocol.cpp
	$(CPP) -c Protocol.cpp -o Protocol.o $(CXXFLAGS)
