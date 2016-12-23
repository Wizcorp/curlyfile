#ifndef NATIVE_EXTENSION_GRAB_H
#define NATIVE_EXTENSION_GRAB_H

#include <nan.h>
#include <curl/curl.h>
#include <list>
#include <mutex>
#include "downloader.h"

#define MAX_CONCURRENT 100

using namespace v8;
using Nan::AsyncQueueWorker;
using Nan::AsyncWorker;
using Nan::Callback;

class DownloadObject;

class Curlyfile : public Nan::ObjectWrap {
  public:
    std::mutex mtx;
    std::list<DownloadObject*> downloads;
    static NAN_MODULE_INIT(Init);
    DownloadObject *GetDownloadObject() {
      mtx.lock();
      DownloadObject *download = downloads.front();
      downloads.pop_front();
      mtx.unlock();

      return download;
    }
    void ReturnDownloadObject(DownloadObject *download) {
      mtx.lock();
      downloads.push_back(download);
      mtx.unlock();
    }
  private:
    explicit Curlyfile();
    ~Curlyfile();

    static NAN_METHOD(New);
    static NAN_METHOD(Download);
    static Nan::Persistent<v8::Function> constructor;
};

void add_download(CURL *session);

class DownloadObject {
  public:
    Curlyfile *curly;
    CURL *session;
    FILE *file;
    char error[256];
    Nan::Callback *callback;
    v8::Local<v8::Value> argv[1];
    DownloadObject(Curlyfile *curly, CURL *session)
      : curly(curly), session(session) {}
    ~DownloadObject(){}
    void Start(char *url, char *outfile, Nan::Callback *callback) {
      error[0] = '\0';
      file = fopen(outfile, "wb");
      if (!file) {
        sprintf(error, "Failed to open destination file: %s", strerror(errno));
        argv[0] = Nan::Error(error);
        callback->Call(1, argv);
        return;
      }

      this->callback = callback;

      curl_easy_setopt(session, CURLOPT_URL, url);
      curl_easy_setopt(session, CURLOPT_WRITEDATA, file);
      add_download(session);
    }
    void OnComplete (int httpCode) {
      if (strlen(error) > 0) {
        argv[0] = Nan::Error(error);
      } else if (httpCode != 200) {
        sprintf(error, "Non-200 return code, received %d", httpCode);
        argv[0] = Nan::Error(error);
      } else {
        argv[0] = Nan::Undefined();
      }

      fclose(file);
      callback->Call(1, argv);
      curly->ReturnDownloadObject(this);
    }
};
#endif
