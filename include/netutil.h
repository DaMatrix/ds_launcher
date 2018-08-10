#ifndef NETUTIL_H
#define NETUTIL_H

#include <misc.h>
#include <buffer.h>

void initNetworking();

char* requestData(char* url);
void downloadFile(Entry entry);

#endif /* NETUTIL_H */

