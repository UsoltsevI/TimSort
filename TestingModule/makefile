all: ../TimSort.o TimSortTestMod.o ../ReadStrings.o
	gcc -O2 ../TimSort.o TimSortTestMod.o ../ReadStrings.o -l m -o TSM.exe
	@echo 'Usage option one: make run'
	@echo 'Usage option two: ./TSM.exe <inputfile> <outputfile>'

../TimSort.o: ../TimSort.c
	gcc -O2 ../TimSort.c -c -o ../TimSort.o

TimSortTestMod.o: TimSortTestMod.c
	gcc -O2 TimSortTestMod.c -c

../ReadStrings.o: ../ReadStrings.c
	gcc -O2 ../ReadStrings.c -c -o ../ReadStrings.o

clean: 
	rm TimSortTestMod.o

gen: 
	gcc -O2 TimSortGenerMod.c -o TGM.exe
	@echo 'Usage: ./TGM.exe <num_elem> <mod> <max_num> <name_file> '

run: 
	./TSM.exe Tests/MainTest.txt Tests/Result.txt
