all:
	gcc -ggdb viewer.c -o viewer
	gcc -ggdb writer.c -o writer
	gcc -ggdb sender.c -o sender -lcurl
	gcc -ggdb webfetch.c -o webfetch -lcurl

clean:
	rm -f viewer writer sender webfetch
