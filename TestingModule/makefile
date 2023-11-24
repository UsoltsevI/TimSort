all: ../timsort.o timsortTestMod.o ../readstrings.o
	gcc -O2 ../timsort.o timsortTestMod.o ../readstrings.o -l m -o TSM.exe
	@echo 'Usage option one: make run'
	@echo 'Usage option two: ./TSM.exe <inputfile> <outputfile>'

../timsort.o: ../timsort.c
	gcc -O2 ../timsort.c -c -o ../timsort.o

timsortTestMod.o: timsortTestMod.c
	gcc -O2 timsortTestMod.c -c

../readstrings.o: ../readstrings.c
	gcc -O2 ../readstrings.c -c -o ../readstrings.o

clean: 
	rm timsortTestMod.o

gen: 
	gcc -O2 TimSortGenerMod.c -o TGM.exe
	@echo 'Usage: ./TGM.exe <num_elem> <mod> <max_num> <name_file> '

run: 
	./TSM.exe Tests/MainTest.txt Tests/Result.txt

TestModStandart.o: TestModStandart.c
	gcc -O2 TestModStandart.c -c

stcompile:
	gcc -O2 TestModStandart.o ../timsort.o -o TMST.exe 
	@echo 'Usage: ./TMST.exe'
	@echo 'And then: <num_elem> <elems> <mod>'
