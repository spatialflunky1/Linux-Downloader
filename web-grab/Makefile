LIBS=\
	 -lcurl

OBJS=\
	 main.o

all: obj web-grab

web-grab: $(OBJS:%=obj/%)
	gcc -o $@ $^ $(LIBS)

obj/%.o: src/%.c
	gcc -c -o $@ $<

obj: 
	mkdir -p obj

clean:
	rm -f $(OBJS:%=obj/%)
	rm -f web-grab
	rmdir obj

obj/main.o: src/main.c
