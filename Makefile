
# The first target is the default if you just say "make".  In this
# case, "build" relies on "sample", because I want the executable to be
# called "sample"

all: addqueue showqueue rmqueue

# "sample" requires a set of object files
# $@ is a special variable: the target of the operation, in this case sample
# $? is the
addqueue: AddQueue.o
	gcc -g -o $@ AddQueue.o -lm
	
showqueue: ShowQueue.o
	gcc -g -o $@ ShowQueue.o -lm
	
rmqueue: RmQueue.o
	gcc -g -o $@ RmQueue.o -lm


test:	
	touch test1.txt
	touch test2.txt
	touch test3.txt
	showqueue
	addqueue test1.txt
	showqueue
	addqueue test2.txt test3.txt
	showqueue
	rmqueue test2.txt
	showqueue
	rmqueue test1.txt test3.txt test4.txt
	showqueue
	rm test1.txt test2.txt test3.txt

clean:
	rm -f addqueue showqueue rmqueue *.core *.o
