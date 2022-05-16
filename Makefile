LIBS=\
	 -lncurses\
	 -ltinfo\
	 -lssl\
	 -lcrypto\
	 -lcurl

OBJS=\
	 network.o\
	 window.o\
	 main.o

all: obj lidown

lidown: $(OBJS:%=obj/%)
	g++ $(LIBS) -o $@ $^

obj/%.o: src/%.cpp
	g++ -c -o $@ $<

obj: 
	mkdir -p obj

clean:
	rm -f $(OBJS:%=obj/%)
	rm -f lidown
	rmdir obj

obj/main.o: src/main.cpp src/network.h src/window.h src/httplib.h
obj/network.o: src/network.cpp src/network.h src/window.h src/httplib.h
obj/window.o: src/window.cpp
