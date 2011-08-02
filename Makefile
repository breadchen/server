
compiler = gcc #arm-linux-gcc

configs = config.h errreport.h
server_files = server.c #$(configs) subserver.h
subserver_files = subserver.c #$(configs) message.h
objects = subserver.o server.o

server : $(objects)
	$(compiler) -g -o server $(objects)
subserver.o : $(subserver_files)
	$(compiler) -g -c $(subserver_files)
server.o : $(server_files)
	$(compiler) -g -c $(server_files)
