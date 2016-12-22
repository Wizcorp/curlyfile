#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <stdio.h>
#include <stdlib.h>
#include <uv.h>
#include <curl/curl.h>
#include "curlyfile.h"

void downloader_init();
void add_download(CURL *session);
#endif

