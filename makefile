all: TimSort.o TimSortTest.o ReadStrings.o
	gcc TimSort.o TimSortTest.o ReadStrings.o -l m -o TS.exe

TimSort.o: TimSort.c
	gcc TimSort.c -c

TimSortTest.o: TimSortTest.c
	gcc TimSortTest.c -c

ReadStrings.o: ReadStrings.c
	gcc ReadStrings.c -c

clean: 
	rm TimSort.o TimSortTest.o ReadStrings.o

run: 
	./TS.exe Tests/MainTest.txt Tests/Result.txt
