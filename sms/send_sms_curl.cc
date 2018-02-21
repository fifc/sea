
// gcc -I/home/pkg/include -L/home/pkg/lib -lcurl
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* somewhat unix-specific */
#include <sys/time.h>
#include <unistd.h>

/* curl stuff */
#include <curl/curl.h>
#include "url_encode.h"
#include "send_sms.h"

static
void dump(const char *text, unsigned char *ptr, size_t size,
          char nohex)
{
  size_t i;
  size_t c;

  unsigned int width=0x10;

  if(nohex)
    /* without the hex output, we can fit more on screen */
    width = 0x40;

  fprintf(stderr, "%s, %ld bytes (0x%lx)\n",
          text, (long)size, (long)size);

  for(i=0; i<size; i+= width) {

    fprintf(stderr, "%4.4lx: ", (long)i);

    if(!nohex) {
      /* hex not disabled, show it */
      for(c = 0; c < width; c++)
        if(i+c < size)
          fprintf(stderr, "%02x ", ptr[i+c]);
        else
          fputs("   ", stderr);
    }

    for(c = 0; (c < width) && (i+c < size); c++) {
      /* check for 0D0A; if found, skip past and start a new line of output */
      if(nohex && (i+c+1 < size) && ptr[i+c]==0x0D && ptr[i+c+1]==0x0A) {
        i+=(c+2-width);
        break;
      }
      fprintf(stderr, "%c",
              (ptr[i+c]>=0x20) && (ptr[i+c]<0x80)?ptr[i+c]:'.');
      /* check again for 0D0A, to avoid an extra \n if it's at width */
      if(nohex && (i+c+2 < size) && ptr[i+c+1]==0x0D && ptr[i+c+2]==0x0A) {
        i+=(c+3-width);
        break;
      }
    }
    fputc('\n', stderr); /* newline */
  }
}

static
int my_trace(CURL *handle, curl_infotype type,
             char *data, size_t size,
             void *userp)
{
  const char *text;
  (void)handle; /* prevent compiler warning */
  (void)userp;
  switch (type) {
  case CURLINFO_TEXT:
    fprintf(stderr, "== Info: %s", data);
  default: /* in case a new one is introduced to shock us */
    return 0;

  case CURLINFO_HEADER_OUT:
    text = "=> Send header";
    break;
  case CURLINFO_DATA_OUT:
    text = "=> Send data";
    break;
  case CURLINFO_SSL_DATA_OUT:
    text = "=> Send SSL data";
    break;
  case CURLINFO_HEADER_IN:
    text = "<= Recv header";
    break;
  case CURLINFO_DATA_IN:
    text = "<= Recv data";
    break;
  case CURLINFO_SSL_DATA_IN:
    text = "<= Recv SSL data";
    break;
  }

  dump(text, (unsigned char *)data, size, 1);
  return 0;
}

#if 0
static const char *g_sms_account = "jiekou-clcs-15";
static const char *g_sms_passwd = "Tch987654";
// static const char *g_balance_uri = "http://222.73.117.158/msg/QueryBalance";
static const char *g_sms_uri = "http://222.73.117.158/msg/HttpBatchSendSM";
#else
static const char *g_sms_account = "jiekou-clcs-15";
static const char *g_sms_passwd = "Tch987654";
// static const char *g_balance_uri = "http://222.73.117.158/msg/QueryBalance";
static const char *g_sms_uri = "http://127.0.0.1/msg/HttpBatchSendSM";
#endif // 0

struct sms_context {
  CURL *curl_;
  int size_;
  char buffer_[256];
};

static size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
	struct sms_context *context = (struct sms_context *)userp;
	int len = size * nmemb;
	if ((unsigned)len >= sizeof context->buffer_ - context->size_)
	  len = sizeof context->buffer_ - context->size_ - 1;
	if (len > 0) {
	  memcpy(context->buffer_, buffer, len);
	  context->size_ += len;
	  context->buffer_[context->size_] = 0;
	}
	return len;
}

static void setup(struct sms_context *context)
{
  CURL *hnd = context->curl_;
  
  /* set the same URL */
  curl_easy_setopt(hnd, CURLOPT_URL, g_sms_uri);

  /* send it verbose for max debuggaility */
  curl_easy_setopt(hnd, CURLOPT_VERBOSE, 1L);
  curl_easy_setopt(hnd, CURLOPT_DEBUGFUNCTION, my_trace);

  /* HTTP/2 please */
  curl_easy_setopt(hnd, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);

  /* we use a self-signed test server, skip verification during debugging */
  curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYPEER, 0L);
  curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYHOST, 0L);

  curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, write_data); 
  curl_easy_setopt(hnd, CURLOPT_WRITEDATA, context); 
}

static int check_response(struct sms_context *context)
{
	char *p = strchr(context->buffer_, '.');
	if (p == NULL) return -1;
	long code = atol(++p);
	return code ? -1 : 0;
}

static int build_data(char *data, int size, const char *phone, const char *msg)
{
	int len = 0;
	len += snprintf(data + len, size - len, "account=");
	len += url_encode(g_sms_account, strlen(g_sms_account), data + len, size -len);
	len += snprintf(data + len, size - len, "&product=");
	len += snprintf(data + len, size - len, "&needstatus=false");
	len += snprintf(data + len, size - len, "&pswd=");
	len += url_encode(g_sms_passwd, strlen(g_sms_passwd), data + len, size -len);
	len += snprintf(data + len, size - len, "&extno=");
	if (phone[0] != 0) {
	  len += snprintf(data + len, size - len, "&mobile=\%2b");
	  int i = 0;
	  if (phone[0] == '+')
	    ++i;
	  for ( ; phone[i] != 0; ++i)
	    if (phone[i] != ' ')
	      data[len++] = phone[i];
	}
	len += snprintf(data + len, size - len, "&msg=");
	len += url_encode(msg, strlen(msg), data + len, size -len);
	return len;
}

namespace sms {
/*
 * Simply download two files over HTTP/2, using the same physical connection!
 */
int SendSM(const std::vector<SmsMsg>& msgs)
{
  struct sms_context context;
  context.curl_ = curl_easy_init();

  setup(&context);

  int rc = 0;
  for (auto& msg : msgs) {
    context.size_ = 0;
    context.buffer_[0] = 0;

    char post_data[1024];
    int post_len = build_data(post_data, sizeof post_data, msg.phone_.c_str(), msg.msg_.c_str());

    struct curl_slist *headers = NULL;
    // char *header = (char*)malloc(32);
    // strcpy(header, "Connection: close");
    // headers = curl_slist_append(headers, header);
    char *header = (char*)malloc(80);
    strcpy(header, "Content-type: application/x-www-form-urlencoded");
    headers = curl_slist_append(headers, header);
    header = (char*)malloc(32);
    strcpy(header, "Accept: text/plain");
    headers = curl_slist_append(headers, header);
    header = (char*)malloc(50);
    snprintf(header, 50, "Content-Length: %d", post_len);
    headers = curl_slist_append(headers, header);

    curl_easy_setopt(context.curl_, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(context.curl_, CURLOPT_POSTFIELDS, post_data);
    curl_easy_setopt(context.curl_, CURLOPT_POSTFIELDSIZE, (long)post_len);

    rc = curl_easy_perform(context.curl_);
    if (rc == CURLE_OK) {
      rc = check_response(&context);
    } else {
      rc = -1;
    }
    curl_slist_free_all(headers);
    //if (rc != 0) break;
    fprintf(stderr, "%s sending 1 msg\n", rc == 0 ? "successfully" : "failed");
    getchar();
  }
  curl_easy_cleanup(context.curl_);

  return rc;
}

int SendSM(const std::string& phone, const std::string& msg)
{
  std::vector<SmsMsg> msgs;
  msgs.emplace_back(phone, msg);
  return SendSM(msgs);
}
} // namespace sms
