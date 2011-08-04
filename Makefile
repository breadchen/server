
compiler = gcc #arm-linux-gcc

configs = config.h errreport.h
pidmanager_files = pidmanager.c
server_files = server.c #$(configs) subserver.h
subserver_files = subserver.c #$(configs) message.h
responsecode_files = responsecode.c 
objects = subserver.o server.o pidmanager.o responsecode.o

server : $(objects) $(configs) 
	$(compiler) -g -o server $(objects)
subserver.o : $(subserver_files) $(configs) message.h
	$(compiler) -g -c $(subserver_files)
server.o : $(server_files) responsecode.h
	$(compiler) -g -c $(server_files)
pidmanager.o : $(pidmanager_files) $(configs)
	$(compiler) -g -c $(pidmanager_files)
responsecode.o : $(responsecode_files) responsecode.h
	$(compiler) -g -c $(responsecode_files)
