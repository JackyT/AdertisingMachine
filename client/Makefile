all:
	gcc -Wall client.c -o client -lcgic
start:
	sudo /var/httpd/bin/httpd -k start
cp:
	sudo cp adver.html /var/httpd/htdocs
	sudo cp *.cgi /var/httpd/cgi-bin

clean:
	rm -f *~
	rm -f *.o 
	rm -f *.swp
	rm -f client
