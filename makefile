all: TimSortIntQ/TimSortTestInt.o timsort.o
	gcc -O2 timsort.o TimSortIntQ/TimSortTestInt.o -o TQI.exe
	@echo 'Usage option 1: make runint'
	@echo 'Usage oprion 2: make runvsq'

TimSortIntQ/TimSortTestInt.o: TimSortIntQ/TimSortTestInt.c
	gcc -O2 TimSortIntQ/TimSortTestInt.c -c -o TimSortIntQ/TimSortTestInt.o

timsort.o: timsort.c
	gcc -O2 timsort.c -c 

runint:
	./TQI.exe -tim_int

runvsq:
	./TQI.exe -tim_vs_q

runsp:
	./TQI.exe -tim_vs_q < TestingModule/Tests/Test4.txt

runisp:
	valgrind ./TQI.exe -tim_int < TestingModule/Tests/Test2.txt
	