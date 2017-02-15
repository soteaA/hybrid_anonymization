# Makefile
# Target_Name: file1 file2 file3..
#
# Basically, target name should be an executable file.
#

# Option List for gcc
#	-g: obtain DEBUG info
#	-c: compile and generate an object file
#	-o test.o: compile and generate test.o, object file
#	-o test: compile and generate test, a linked one

# .PHONY for avoiding any errors caused by the phony target name 'clean'
# By .PHONY, any file named as 'clean' could exist in the same directory

check: check.c
	gcc -g -O2 $^ -o $@ -lm

.PHONY: clean
clean:
	rm -rf *.o *~ check
