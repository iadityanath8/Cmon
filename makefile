CC = cc
Cperf = -O3
Cflags = -Wall -Wextra -Wnonnull -pedantic


run:
	$(CC) $(Cflags) -o cmon mm.c && ./cmon

comp:
	$(CC) $(Cflags) -o cmon mm.c

clean:
	rm -rf ./cmon
