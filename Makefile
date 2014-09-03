CC=g++

kilocmd: kilocmd.cc intelhex.o
	$(CC) -Wall -Wextra -std=c++11 -O3 -flto -lftdi -o kilocmd intelhex.o kilocmd.cc

intelhex.o: intelhex.cc intelhex.h
	$(CC) -Wall -Wextra -std=c++11 -O3 -flto -c intelhex.cc

clean:
	rm -f kilocmd intelhex.o
