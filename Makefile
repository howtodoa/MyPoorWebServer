all: myhttp

myhttp: httpd.c
	gcc -W -Wall -o3 myhttp httpd.c -lpthread

clean:
	rm myhttp
