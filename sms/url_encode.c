
#include "url_encode.h"

int url_encode(const char *in, int in_size, char *out, int out_size)
{
	static char map[] = {
	  '\x00', '\x00', '\x00', '\x00', '\x00', '\x60', '\xff', '\x03',
	  '\xfe', '\xff', '\xff', '\x87', '\xfe', '\xff', '\xff', '\x47',
	  '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
	  '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00'
	};
	int len = 0;
	for (int i = 0; i < in_size; ++i) {
	  if (map[in[i]/8]&(1<<(in[i]%8)))
	    out[len++] = in[i];
	  else {
	    out[len++] = '%';
	    out[len++] = "0123456789abcdef"[((unsigned char)in[i])>>4];
	    out[len++] = "0123456789abcdef"[in[i]&0x0f];
	  }
	}
	return len;
}
