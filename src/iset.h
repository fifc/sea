#ifndef __ISET_H_INCLUDED__
#define __ISET_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif 

#include <stdlib.h>

typedef struct iset_st_ {
	long num_;
	unsigned long* seg_;
} iset;

unsigned long iset_map(iset* is, int (*handler)(unsigned long*, void *), void* data);

int iset_has(iset* is, unsigned long value);

void iset_add(iset* ids, unsigned long lo, unsigned long hi);

inline void iset_insert(iset* ids, unsigned long value) {
	iset_add(ids, value, value + 1);
}

inline void iset_init(iset* ids) {
	ids->num_ = 0;
}

unsigned long iset_sub(iset* ids, unsigned long start, unsigned long total, iset* res);

inline void iset_free(iset* is) {
	if (is->num_ != 0) {
		free(is->seg_);
		is->num_ = 0;
		is->seg_ = NULL;
	}
}

inline int iset_erase(iset* is, unsigned long value) {
	iset res;
	iset_sub(is, value, 1, &res);
	unsigned long num = res.num_;
	iset_free(&res);
	return num == 1 ? 1 : 0;
}

unsigned long iset_get(iset* is);

inline unsigned long* iset_seg(iset *is, unsigned long index) {
	return index < is->num_ ? (is->seg_ + (index<<1lu)) : NULL;
}

int iset_select(iset* ids, unsigned long id);

unsigned long iset_start(iset* ids, unsigned long start);


#ifdef __cplusplus
}
#endif 
#endif // __ISET_H_INCLUDED__
