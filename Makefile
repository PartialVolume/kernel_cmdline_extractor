CC=gcc
CFLAGS="-Wall"

debug:clean
	$(CC) $(CFLAGS) -g -o kernel_cmdline_extractor main.c
stable:clean
	$(CC) $(CFLAGS) -o kernel_cmdline_extractor main.c
clean:
	rm -vfr *~ kernel_cmdline_extractor
