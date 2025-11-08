all : testtop

# to make CI happy.
testtop.hidapi.elf : testtop.c
	gcc -o $@ $^ -I../../../../minichlink -ludev

testtop : testtop.c
	gcc -o $@ $^ -I../../../../minichlink -ludev

clean :
	rm -rf *.o *~ testtop

