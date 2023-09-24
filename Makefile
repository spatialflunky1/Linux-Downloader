LIBS=\
	 -lncurses\
	 -ltinfo\
	 -lcurl

OBJS=\
	 window.o\
	 network.o\
	 main.o

all: obj lidown

lidown: $(OBJS:%=obj/%)
	gcc -o $@ $^ $(LIBS)

obj/%.o: src/%.c
	gcc -c -o $@ $<

obj: 
	mkdir -p obj

clean:
	rm -f $(OBJS:%=obj/%)
	rm -f lidown
	rmdir obj

obj/main.o: src/main.c src/window.h src/network.h
obj/window.o: src/window.c
obj/network.o: src/network.c
