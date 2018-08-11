#ifndef NETUTIL_H
#define NETUTIL_H

#include <buffer.h>
#include <misc.h>
#include <registry.h>

void initNetworking();

char* requestIndex();
void downloadFile(Entry* entry);

#endif /* NETUTIL_H */

