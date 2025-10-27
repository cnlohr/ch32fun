all : testtop

# to make CI happy.
testtop.bulk.elf : testtop.c
	gcc -o $@ $^ -I../../../minichlink -ludev -lusb-1.0

testtop : testtop.c
	gcc -o $@ $^ -I../../../minichlink -ludev -lusb-1.0


clean :
	rm -rf *.o *~ testtop

