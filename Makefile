
objects = server.c config.h errreport.h

server : $(objects)
	gcc -g -o server $(objects)
