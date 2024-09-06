#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <stddef.h>
#include "util.h"



void download( const char *url, struct MemoryStruct *mem);

int is_url_active(const char *url);


#endif // DOWNLOAD_H
