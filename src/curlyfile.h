#ifndef NATIVE_EXTENSION_GRAB_H
#define NATIVE_EXTENSION_GRAB_H

#include <nan.h>
#include <curl/curl.h>
#include <list>
#include <future>
#include "downloader.h"

#define MAX_CONCURRENT 50

using namespace v8;
using Nan::AsyncQueueWorker;
using Nan::AsyncWorker;
using Nan::Callback;

class Curlyfile;

class DownloadObject {
  public:
    Curlyfile *curly;
    CURL *session;
    FILE *file;
    char error[256];
    Nan::Callback *callback;
    DownloadObject(Curlyfile *curly, CURL *session)
      : curly(curly), session(session) {
        error[0] = '\0';
      }
    ~DownloadObject(){}

  protected:
    Persistent<v8::Object> persistentHandle;
};

class Curlyfile : public Nan::ObjectWrap {
  public:
    std::list<DownloadObject*> downloads;
    static NAN_MODULE_INIT(Init);

  private:
    explicit Curlyfile();
    ~Curlyfile();

    static NAN_METHOD(New);
    static NAN_METHOD(Download);
    static Nan::Persistent<v8::Function> constructor;
};

class CurlyfileAsyncWorker : public Nan::AsyncWorker {
  public:
    CurlyfileAsyncWorker(Nan::Callback *callback, Curlyfile *curly, CURL *session, char *url, char *outfile)
      : Nan::AsyncWorker(callback), curly(curly), session(session), url(url), outfile(outfile) {}
    ~CurlyfileAsyncWorker(){}
    void Execute();
  protected:
    Curlyfile *curly;
    CURL *session;
    char *url;
    char *outfile;
    char error[256];
    void HandleOKCallback();
};
#endif
