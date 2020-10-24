CC=gcc

bin/vida: obj/util.o obj/main.o
	$(CC) -Wall -g obj/util.o obj/main.o -o bin/vida -fsanitize=address,undefined

obj/util.o: src/util.c include/util.h
	$(CC) -Wall -g -c -I include/ src/util.c -o obj/util.o -fsanitize=address,undefined

obj/main.o: src/main.c src/util.c
	$(CC) -Wall -g -c -I include/ src/main.c -o obj/main.o -fsanitize=address,undefined

.PHONY: clean
clean:
	rm -f obj/*.o bin/vida

