all: ../timsort.o TimSortTest.o ../readstrings.o
	gcc -O2 ../timsort.o TimSortTest.o ../readstrings.o -l m -o TS.exe
	@echo 'Usage option one: make run'
	@echo 'Usage option two: ./TS.exe <inputfile> <outputfile>'

../timsort.o: ../timsort.c
	gcc -O2 ../timsort.c -c -o ../timsort.o

TimSortTest.o: TimSortTest.c
	gcc -O2 TimSortTest.c -c

../readstrings.o: ../readstrings.c
	gcc -O2 ../readstrings.c -c -o ../readstrings.o

clean: 
	rm timsortTest.o

compilegen: 
	gcc -O2 timsortGener.c -o TG.exe
	@echo 'Usage: ./TG.exe <num_sets> <num_elem_in_set> <field_num> <name_file> '

run: 
	./TS.exe Tests/MainTest.txt Tests/Result.txt
