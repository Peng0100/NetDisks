#ifndef HEADER_H
#define HEADER_H

#include <unistd.h>
#include <sys/types.h>                  // for ftruncate state

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <signal.h>

#include <sys/time.h>
#include <time.h>

#include <arpa/inet.h>

#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/uio.h>

#include <mysql/mysql.h>

#include <iostream>
#include <vector>
#include <map>
#include <queue>

using namespace std;

#define ErrorCheck(val, ret, funName) {if(ret==val){perror(funName); exit(1); }}

#endif
