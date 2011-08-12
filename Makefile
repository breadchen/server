
compiler = gcc#arm-linux-gcc

configs = config.h errreport.h
pidmanager_files = pidmanager.c
server_files = server.c 
subserver_files = subserver.c 
responsecode_files = responsecode.c 
objects = subserver.o server.o pidmanager.o responsecode.o \
executecommand.o commanddata.o

server : $(objects) $(configs) 
	$(compiler) -g -o server $(objects)
subserver.o : $(subserver_files) $(configs) message.h executecommand.h \
responsecode.h
	$(compiler) -g -c $(subserver_files)
server.o : $(server_files) responsecode.h
	$(compiler) -g -c $(server_files)
pidmanager.o : $(pidmanager_files) $(configs)
	$(compiler) -g -c $(pidmanager_files)
responsecode.o : $(responsecode_files) responsecode.h
	$(compiler) -g -c $(responsecode_files)
executecommand.o : executecommand.c executecommand.h $(configs) \
responsecode.h commanddata.h
	$(compiler) -g -c executecommand.c
commanddata.o : commanddata.c commanddata.h
	$(compiler) -g -c commanddata.c

.PHONY : clean
clean : 
	-rm server $(objects)
