all: ../TimSort.o TimSortTest.o ../ReadStrings.o
	gcc -O2 ../TimSort.o TimSortTest.o ../ReadStrings.o -l m -o TS.exe
	@echo 'Usage option one: make run'
	@echo 'Usage option two: ./TS.exe <inputfile> <outputfile>'

../TimSort.o: ../TimSort.c
	gcc -O2 ../TimSort.c -c -o ../TimSort.o

TimSortTest.o: TimSortTest.c
	gcc -O2 TimSortTest.c -c

../ReadStrings.o: ../ReadStrings.c
	gcc -O2 ../ReadStrings.c -c -o ../ReadStrings.o

clean: 
	rm TimSortTest.o

compilegen: 
	gcc -O2 TimSortGener.c -o TG.exe
	@echo 'Usage: ./TG.exe <num_sets> <num_elem_in_set> <field_num> <name_file> '

run: 
	./TS.exe Tests/MainTest.txt Tests/Result.txt
