#ifndef NETUTIL_H
#define NETUTIL_H

#include <buffer.h>
#include <misc.h>
#include <registry.h>

void initNetworking();

char* requestData(char* url);
void downloadFile(Entry* entry);

#endif /* NETUTIL_H */

