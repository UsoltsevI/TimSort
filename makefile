all: timsort.o readstrings.o
	@echo 'you can use following options:'
	@echo 'int, mod, set, set2'
	@echo 'For example: make int'

timsort.o: timsort.c
	gcc -O2 timsort.c -c 

readstrings.o: readstrings.c 
	gcc -O2 readstrings.c -c
	
clean:
	rm *.exe *.o

cleanall:
	rm *.exe *.o TestingInt/*.o TestingModule/*.o TestingSets/*.o TestingSetsTwo/*.o

int: TestingInt/TimSortTestInt.o timsort.o
	gcc -O2 timsort.o TestingInt/TimSortTestInt.o -o TI.exe
	@echo 'Usage option 1 (sort int array): make intrun'
	@echo 'Usage option 2 (sort int array): ./TI.exe -tim_int'
	@echo 'Usage option 3 (compare with qsort): make intrunvsq'
	@echo 'Usage option 4 (compare with qsort): ./TI.exe -tim_vs_q'

TestingInt/TimSortTestInt.o: TestingInt/TimSortTestInt.c
	gcc -O2 TestingInt/TimSortTestInt.c -c -o TestingInt/TimSortTestInt.o

intrun:
	./TI.exe -tim_int

intrunvsq:
	./TI.exe -tim_vs_q

intclean:
	rm TestingInt/TimSortTestInt.o


mod: timsort.o readstrings.o TestingModule/TimSortTestMod.o
	gcc -O2 timsort.o readstrings.o TestingModule/TimSortTestMod.o -l m -o TM.exe 
	@echo 'Usage option 1: make modrun'
	@echo 'Usage option 2: ./TM.exe <inputfile> <outputfile>'

TestingModule/TimSortTestMod.o: TestingModule/TimSortTestMod.c 
	gcc -O2 TestingModule/TimSortTestMod.c -c -o TestingModule/TimSortTestMod.o

modclean:
	rm TestingModule/TimSortTestMod.o TestingModule/TimSortGenerMod.o

modgen: TestingModule/TimSortGenerMod.c 
	gcc -O2 TimSortGenerMod.c -o TGM.exe 
	@echo 'Usage option 1: ./TGM.exe <num_elem> <mod> <max_num> <name_file>'

modrun: 
	./TM.exe TestingModule/Tests/MainTest.txt TestingModule/Tests/Result.txt
	
modst:
	gcc -O2 TestingModule/TestModStandart.o timsort.o -o TMST.exe 
	@echo 'Usage: ./TMST.exe'
	@echo 'Requirements for input data: <num_elem> <elems> <mod>'


set: timsort.o readstrings.o TestingSets/TimSortTest.o 
	gcc -O2 timsort.o readstrings.o TestingSets/TimSortTest.o -l m -o TS.exe
	@echo 'Usage option 1: make setrun'
	@echo 'Usage option 2: ./TS.exe <inputfile> <outputfile>'

TestingSets/TimSortTest.o: TestingSets/TimSortTest.c 
	gcc -O2 TestingSets/TimSortTest.c -c -o TestingSets/TimSortTest.o

setclean:
	rm TestingSets/TimSortTest.o TestingSets/TimSortGener.o

setgen:
	gcc -O2 TestingSets/TimSortGener.c -o TGS.exe 
	@echo 'Usage option 1: ./TGS.exe <num_sets> <num_elem_in_set> <field_num> <name_file>'

setrun:
	./TS.exe TestingSets/Tests/MainTest.txt TestingSets/Tests/Result.txt


set2: timsort.o readstrings.o TestingSetsTwo/TimSortTest.o 
	gcc -O2 timsort.o readstrings.o TestingSetsTwo/TimSortTest.o -l m -o TST.exe
	@echo 'Usage option 1: make set2run'
	@echo 'Usage option 2: ./TST.exe <inputfile> <outputfile>'

TestingSetsTwo/TimSortTest.o: TestingSetsTwo/TimSortTest.c 
	gcc -O2 TestingSetsTwo/TimSortTest.c -c -o TestingSetsTwo/TimSortTest.o

set2clean:
	rm TestingSetsTwo/TimSortTest.o TestingSetsTwo/TimSortGener.o

set2gen:
	gcc -O2 TestingSetsTwo/TimSortGener.c -o TGST.exe 
	@echo 'Usage option 1: ./TGST.exe <num_sets> <num_elem_in_set> <field_num> <name_file>'

set2run:
	./TST.exe TestingSetsTwo/Tests/MainTest.txt TestingSetsTwo/Tests/Result.txt
