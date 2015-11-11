
# The first target is the default if you just say "make".
build: addqueue showqueue rmqueue

addqueue: AddQueue.o
	gcc -g -o $@ AddQueue.o -lm
	
showqueue: ShowQueue.o
	gcc -g -o $@ ShowQueue.o -lm
	
rmqueue: RmQueue.o
	gcc -g -o $@ RmQueue.o -lm


test:	
	touch /tmp/test1.txt
	touch /tmp/test2.txt
	touch /tmp/test3.txt
	showqueue
	@echo "--------"
	addqueue /tmp/test1.txt
	@echo "--------"
	showqueue
	@echo "--------"
	addqueue /tmp/test2.txt /tmp/test3.txt
	@echo "--------"
	showqueue
	@echo "--------"
	rmqueue test2.txt_2
	@echo "--------"
	showqueue
	@echo "--------"
	rmqueue test1.txt_1 test3.txt_3 test4.txt_4
	@echo "--------"
	showqueue
	rm /tmp/test1.txt /tmp/test2.txt /tmp/test3.txt

clean:
	rm -f addqueue showqueue rmqueue *.core *.o
