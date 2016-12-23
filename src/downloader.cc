#include "downloader.h"

CURLM *curl_handle;
uv_loop_t *loop;
uv_timer_t timeout;

typedef struct curl_context_s {
  uv_poll_t poll_handle;
  curl_socket_t sockfd;
} curl_context_t;

curl_context_t* create_curl_context(curl_socket_t sockfd) {
  curl_context_t *context;

  context = (curl_context_t *) malloc(sizeof *context);
  context->sockfd = sockfd;

  uv_poll_init_socket(loop, &context->poll_handle, sockfd);
  context->poll_handle.data = context;

  return context;
}

void curl_close_cb(uv_handle_t *handle) {
  curl_context_t *context = (curl_context_t *) handle->data;
  free(context);
}

void destroy_curl_context(curl_context_t *context) {
  uv_close((uv_handle_t *) &context->poll_handle, curl_close_cb);
}

static void check_multi_info(void) {
  Nan::HandleScope scope;
  DownloadObject *download;
  CURLMsg *message;
  int pending;
  int httpCode;
  CURL *easy_handle;

  while((message = curl_multi_info_read(curl_handle, &pending))) {
    switch(message->msg) {
    case CURLMSG_DONE:
      /* Do not use message data after calling curl_multi_remove_handle() and
         curl_easy_cleanup(). As per curl_multi_info_read() docs:
         "WARNING: The data the returned pointer points to will not survive
         calling curl_multi_cleanup, curl_multi_remove_handle or
         curl_easy_cleanup." */
      easy_handle = message->easy_handle;
      curl_easy_getinfo(easy_handle, CURLINFO_PRIVATE, &download);
      curl_easy_getinfo(easy_handle, CURLINFO_RESPONSE_CODE, &httpCode);

      download->OnComplete(httpCode);

      curl_multi_remove_handle(curl_handle, easy_handle);
      break;

    default:
      // Todo: throw?
      fprintf(stderr, "CURLMSG default\n");
      break;
    }
  }
}

void curl_perform(uv_poll_t *req, int status, int events) {
  int running_handles;
  int flags = 0;
  curl_context_t *context;

  if(events & UV_READABLE) {
    flags |= CURL_CSELECT_IN;
  }
  if(events & UV_WRITABLE) {
    flags |= CURL_CSELECT_OUT;
  }

  context = (curl_context_t *) req->data;
  curl_multi_socket_action(curl_handle, context->sockfd, flags, &running_handles);
  check_multi_info();
}

void on_timeout(uv_timer_t *req) {
  int running_handles;
  curl_multi_socket_action(curl_handle, CURL_SOCKET_TIMEOUT, 0, &running_handles);
  check_multi_info();
}

void start_timeout(CURLM *multi, long timeout_ms, void *userp) {
  // 0 means directly call socket_action,
  // but we'll do it in a bit
  if(timeout_ms <= 0) {
    timeout_ms = 1;
  }

  uv_timer_start(&timeout, on_timeout, timeout_ms, 0);
}

int handle_socket(CURL *easy, curl_socket_t s, int action, void *userp, void *socketp) {
  curl_context_t *curl_context;
  int events = 0;

  switch(action) {
  case CURL_POLL_IN:
  case CURL_POLL_OUT:
  case CURL_POLL_INOUT:
    curl_context = socketp ?
      (curl_context_t *) socketp : create_curl_context(s);

    curl_multi_assign(curl_handle, s, (void *) curl_context);

    if(action != CURL_POLL_IN) {
      events |= UV_WRITABLE;
    }

    if(action != CURL_POLL_OUT) {
      events |= UV_READABLE;
    }

    uv_poll_start(&curl_context->poll_handle, events, curl_perform);
    break;
  case CURL_POLL_REMOVE:
    if(socketp) {
      uv_poll_stop(&((curl_context_t*)socketp)->poll_handle);
      destroy_curl_context((curl_context_t*) socketp);
      curl_multi_assign(curl_handle, s, NULL);
    }
    break;
  default:
    abort();
  }

  return 0;
}

void add_download(CURL *session) {
  curl_multi_add_handle(curl_handle, session);
}

void downloader_init() {
  loop = uv_default_loop();
  curl_global_init(CURL_GLOBAL_ALL);
  uv_timer_init(loop, &timeout);

  curl_handle = curl_multi_init();

  curl_multi_setopt(curl_handle, CURLMOPT_SOCKETFUNCTION, handle_socket);
  curl_multi_setopt(curl_handle, CURLMOPT_TIMERFUNCTION, start_timeout);
}
