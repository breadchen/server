#ifndef _ADDRESS_H
#define _ADDRESS_H

#define PORT 2000
#define IP "192.168.2.3"
#define LEAST_ARG_NUM 2
#define BACK_LOG 5
// server will only read the frist MAX_MSG_LEN
// chars of each message 
#define MAX_MSG_LEN 50
// use this to init pid_clients[]
#define MAX_CLIENT_NUM 256
// char buffer size, used to convert int to char[]
#define BUF_SIZE 50

#define FUC_SUCCESS 0
#define FUC_FAILURE 1

#endif // if (!_ADDRESS_H)
