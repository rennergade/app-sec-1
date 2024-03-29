default: prog

get-deps:
	# Assuming Debian or Ubuntu here
	sudo apt-get update
	sudo apt-get install -y build-essential check

dictionary.o: dictionary.c
	gcc -Wall -g -c dictionary.c dictionary.h

spell.o: spell.c
	gcc -Wall -g -c spell.c

test.o: test_main.c
	gcc -Wall -g -c test_main.c

main.o: main.c
	gcc -Wall -g -c main.c

test: dictionary.o spell.o test_main.o
	gcc -Wall -o test_main test_main.o spell.o dictionary.o -lcheck -lm -lrt -lpthread -lsubunit
	./test_main

prog: dictionary.o spell.o main.o
	gcc -Wall -g -o spell_check dictionary.o spell.o main.o

fuzz: 
	export AFL_INST_RATIO=100 
	export AFL_HARDEN=1 
	./afl/afl-gcc -Wall -o fuzz_main main.c spell.c dictionary.c -lcheck -lm -lrt -lpthread -lsubunit
	./afl/afl-fuzz -i in -o out ./fuzz_main small_wordlist.txt @@


clean:
	rm *.o specll_check test_main fuzz_main

cleanall:clean
	rm spell_check
	
