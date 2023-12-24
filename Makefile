all: myhttp

myhttp: httpd.cc
	g++ -std=c++17 -o myhttp httpd.cc -lpthread

clean:
	rm myhttp
