
configs = config.h errreport.h
server_files = server.c $(configs) subserver.h
subserver_files = subserver.c $(configs)
objects = subserver.o server.o

server : $(objects)
	gcc -g -o server $(objects)
subserver.o : $(subserver_files)
	gcc -g -c $(subserver_files)
server.o : $(server_files)
	gcc -g -c $(server_files)
