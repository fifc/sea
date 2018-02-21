#ifndef _URL_ENCODE_H_
#define _URL_ENCODE_H_

#if defined(__cplusplus)
extern "C" {
#endif // cplusplus

int url_encode(const char *in, int in_size, char *out, int out_size);

#if defined(__cplusplus)
}
#endif // cplusplus

#endif // _URL_ENCODE_H_
