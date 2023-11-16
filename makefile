all: TimSort.o TestTimsortTwo.o ReadStrings.o
	gcc TimSort.o TestTimsortTwo.o ReadStrings.o -l m -o TS.exe

TimSort.o: TimSort.c
	gcc TimSort.c -c

TestTimsortTwo.o: TestTimsortTwo.c
	gcc TestTimsortTwo.c -c

ReadStrings.o: ReadStrings.c
	gcc ReadStrings.c -c

clean: 
	rm TimSort.o TestTimsortTwo.o ReadStrings.o

run: 
	./TS.exe Tests/MainTest.txt Tests/Result.txt
