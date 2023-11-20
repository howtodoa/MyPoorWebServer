all: myhttp

myhttp: httpd.c
	gcc -W -Wall -o1 myhttp httpd.c -lpthread

clean:
	rm myhttp
