
bin/output.exe: bin/WaveEditor.o bin/WaveIO.o
	gcc bin/WaveEditor.o bin/WaveIO.o -o bin/output.exe

bin/WaveEditor.o: WaveEditor.c
	gcc -c WaveEditor.c -o bin/WaveEditor.o

bin/WaveIO.o: lib/WaveIO.c lib/WaveIO.h
	gcc -c lib/WaveIO.c -o bin/WaveIO.o

clean:
	rm *.o