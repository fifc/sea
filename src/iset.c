#include <stdio.h>
#include <string.h>

#include "iset.h"

unsigned long iset_map(iset* is, int (*handler)(unsigned long*, void *), void* data) {
	unsigned long i = 0;
	for ( ; i < is->num_; ++i) {
		if (handler(is->seg_ + (i << 1lu), data))
			break;
	}

	return i;
}

int iset_has(iset* is, unsigned long value) {
	unsigned long i = 0;
	unsigned long j = is->num_;

	while (i<j) {
		unsigned long m = i+((j-i)>>1);
		if (value >= is->seg_[(m << 1lu)+1]) {
			i = m+1;
		} else if (value < is->seg_[m << 1lu]) {
			j = m;
		} else {
			return 1;
		}
	}

	return 0;
}

void iset_add(iset* ids, unsigned long lo, unsigned long hi) {
	unsigned long start, end;
	for (start = 0; start < ids->num_; ++start)
		if (ids->seg_[start<<1] >= lo)
			break;
	for (end = 0; end < ids->num_; ++end)
		if (ids->seg_[(end<<1)+1] >= hi)
			break;
	if (start == ids->num_) {
		if (ids->num_ == 0) {
			ids->seg_ = (unsigned long*)malloc(sizeof(unsigned long) << 1);
			if (ids->seg_ != NULL) {
				ids->seg_[0] = lo;
				ids->seg_[1] = hi;
				ids->num_ = 1;
			}
			return;
		}
		start = ((ids->num_ - 1) << 1)+1;
		if (lo <= ids->seg_[start]) {
			ids->seg_[start] = hi;
			return;
		}
		start = ids->num_++;
		start <<= 1;
		void *seg = realloc(ids->seg_, (ids->num_<<1) * sizeof(unsigned long));
		if (seg != NULL) {
			ids->seg_ = (unsigned long*)seg;
			ids->seg_[start++] = lo;
			ids->seg_[start] = hi;
		}
		return;
	}

	if (end != ids->num_) {
		if (hi >= ids->seg_[end << 1]) {
			hi = ids->seg_[(end << 1) + 1];
			++end;
		}
	}
	if (start > 0 && lo <= ids->seg_[((start-1)<<1)+1]) {
		--start;
	}
	else if (lo < ids->seg_[start << 1]) {
		if (hi < ids->seg_[start << 1]) {
			unsigned long *seg = (unsigned long*)malloc(((ids->num_+1lu)<<1lu) * sizeof(unsigned long));
			if (seg != NULL) {
				if (start > 0)
					memmove(seg, ids->seg_, (start << 1lu) * sizeof(unsigned long));
				seg[start << 1] = lo;
				seg[(start << 1) + 1] = hi;
				memmove(seg + (start << 1lu) + 2, ids->seg_ + (start << 1lu), ((ids->num_ - (unsigned long)start) << 1lu) * sizeof(unsigned long));
				free(ids->seg_);
				ids->seg_ = seg;
				++ids->num_;
			}
			return;
		}

		ids->seg_[start << 1] = lo;
	}

	ids->seg_[(start << 1) + 1] = hi;

	unsigned long num = ids->num_;
	ids->num_ = num - end + start + 1;
	if (ids->num_ < num) {
		start = ((start + 1) << 1);
		unsigned long* seg = (unsigned long*)malloc((ids->num_ << 1lu) * sizeof(unsigned long));
		if (seg != NULL) {
			memmove(seg, ids->seg_, (unsigned long)start * sizeof(unsigned long));
			if (end < num)
				memmove(seg + start, ids->seg_ + (end << 1lu), ((num - end) << 1lu) * sizeof(unsigned long));
			free(ids->seg_);
			ids->seg_ = seg;
		}

	}
}

unsigned long iset_sub(iset* ids, unsigned long start, unsigned long total, iset* res) {
	iset_init(res);
	unsigned long i = 0;
	unsigned long left = total;
	for (; i < ids->num_; ++i) {
		if (start < ids->seg_[(i << 1) + 1]) {
			if (start <= ids->seg_[i << 1])
				break;

			unsigned long num = ids->seg_[(i << 1) + 1] - start;
			ids->seg_[(i << 1) + 1] = start;
			if (num < total) {
				iset_add(res, start, start + num);
				left -= num;
				++i;
				break;
			}

			iset_add(res, start, start + total);

			if (num > total)
				iset_add(ids, start + total, start + num);

			return total;
		}
	}

	unsigned long j = i;
	for ( ; j < ids->num_ && left > 0; ++j) {
		unsigned long lo = ids->seg_[(j << 1)];
		unsigned long num = ids->seg_[(j << 1) + 1] - lo;
		if (num < left) {
			iset_add(res, lo, lo + num);
			left -= num;
		}
		else {
			iset_add(res, lo, lo + left);
			if (num > left)
				ids->seg_[j<<1] = lo + left;
			else
				++j;

			break;
		}
	}

	if (i < j) {
		unsigned long num = ids->num_ - j + i;
		unsigned long* seg = (unsigned long*)malloc((sizeof(unsigned long) << 1) * num);
		memmove(seg, ids->seg_, (i << 1lu) * sizeof(unsigned long));
		memmove(seg + (i<<1lu), ids->seg_ + (j << 1lu), ((ids->num_ - (unsigned long)j) << 1lu) * sizeof(unsigned long));
		ids->num_ = num;
		free(ids->seg_);
		ids->seg_ = seg;
	}

	return total - left;
}

unsigned long iset_get(iset* ids) {
	unsigned long id = ids->seg_[0]++;
	if (ids->seg_[0] == ids->seg_[1]) {
		--ids->num_;
		void* p = malloc(ids->num_ * sizeof(unsigned long) * 2);
		memcpy(p, ids->seg_ + 2lu, ids->num_ * sizeof(unsigned long) * 2);
		free(ids->seg_);
		ids->seg_ = (unsigned long*)p;
	}
	return id;
}

static inline int print_set(unsigned long* isv, void* data) {
	printf("%lu,%lu ", isv[0], isv[1]);
	return 0;
}

static void iset_dump(iset* is) {
	unsigned long i = iset_map(is, print_set, NULL);
	if (i > 0)
		printf("\n");
}

int iset_select(iset* ids, unsigned long id) {
	int i = 0;
	for (; i < ids->num_; ++i) {
		if (id >= ids->seg_[i << 1] && id < ids->seg_[(i << 1) + 1])
			break;
	}

	if (i == ids->num_)
		return 0;

	if (id == ids->seg_[i << 1]) {
		++ids->seg_[i << 1];
		if (ids->seg_[i << 1] != ids->seg_[(i << 1) + 1]) {
			return id;
		}
		if (--ids->num_ == 0) {
			free(ids->seg_);
			ids->seg_ = NULL;
			return id;
		}
		unsigned long* p = (unsigned long*)malloc(ids->num_ * (sizeof(unsigned long) << 1));
		if (i > 0)
			memcpy(p, ids->seg_, i * (sizeof(unsigned long) << 1));
		if (i < ids->num_)
			memcpy(p + (i << 1lu), ids->seg_ + (i << 1lu) + 2, sizeof(unsigned long) * (ids->num_ - i) << 1lu);
		free(ids->seg_);
		ids->seg_ = p;
		return id;
	}

	if (id + 1 == ids->seg_[(i << 1) + 1]) {
		--ids->seg_[(i << 1) + 1];
		return id;
	}

	++ids->num_;
	unsigned long* p = (unsigned long*)malloc(ids->num_ * (sizeof(unsigned long) << 1));
	if (i > 0)
		memcpy(p, ids->seg_, i * (sizeof(unsigned long) << 1));
	p[i << 1lu] = ids->seg_[i << 1lu];
	p[(i << 1lu) + 1] = id;
	++i;
	p[i << 1] = id + 1;
	p[(i << 1) + 1] = ids->seg_[(i << 1) - 1];

	if (i < ids->num_ - 1)
		memcpy(p + (i << 1lu) + 2, ids->seg_ + (i << 1lu), sizeof(unsigned long) * ((ids->num_ - 1lu - i) << 1));
	free(ids->seg_);
	ids->seg_ = p;

	return id;
}

unsigned long iset_start(iset* ids, unsigned long start) {
	int i = 0;
	for (; i < ids->num_; ++i) {
		if (start < ids->seg_[(i << 1) + 1]) {
			if (start < ids->seg_[i << 1]) {
				start = ids->seg_[i << 1];
			}
			return iset_select(ids, start);
		}
	}

	return 0;
}

#ifdef ISET_UNIT_TEST
int main() {
	iset ids;
	iset_init(&ids);
#if 0
	iset_add(&ids, 0, 100);
	iset_start(&ids, 30);
	iset_select(&ids, 2);
	iset_select(&ids, 50);
	iset_select(&ids, 40);
	iset_select(&ids, 80);
	iset_select(&ids, 1);
	iset_select(&ids, 10);
	iset_select(&ids, 98);
	iset_select(&ids, 99);
	iset_start(&ids, 97);
	iset_start(&ids, 90);
	iset_start(&ids, 90);
	iset_start(&ids, 90);
	iset_start(&ids, 90);
	iset_start(&ids, 90);
	iset_start(&ids, 90);
	iset_start(&ids, 90);
	int i = 0;
	for (; i < 8; ++i)
		iset_get(&ids);
	iset_add(&ids, 30, 31);
	iset_add(&ids, 100, 198);
	iset_add(&ids, 96, 97);
	iset_add(&ids, 92, 97);
	iset_add(&ids, 97,201);
	iset_dump(&ids);
	printf("-----------------------------------------\n");
	iset res;
	iset_sub(&ids, 83,109,&res);
	iset_dump(&ids);
	printf("-----------------------------------------\n");
	iset_dump(&res);
	iset_free(&res);
#endif

	iset_add(&ids, 1000000, 10000000);

	char out[40];
	printf("%lu\n",iset_start(&ids, 1200000));
	printf("%lu\n",iset_start(&ids, 1200000));
	printf("%lu\n",iset_start(&ids, 1200000));
	printf("%lu\n",iset_start(&ids, 1100000));
	printf("%lu\n",iset_start(&ids, 1100000));
	printf("%lu\n",iset_start(&ids, 100000));
	printf("%lu\n",iset_start(&ids, 100000));
	printf("%lu\n",iset_start(&ids, 2000000));
	printf("%lu\n",iset_start(&ids, 2000000));
	printf("%lu\n",iset_start(&ids, 2000000));
	printf("%lu\n",iset_start(&ids, 2000000));
	printf("%lu\n",iset_start(&ids, 2000000));
	printf("%lu\n",iset_start(&ids, 2000000));
	printf("%lu\n",iset_start(&ids, 2000008));
	printf("%lu\n",iset_start(&ids, 1200005));
	printf("%lu\n",iset_start(&ids, 1200005));
	printf("%lu\n",iset_start(&ids, 1200005));
	iset_dump(&ids);
	printf("%lu\n",iset_start(&ids, 1200003));
	iset_dump(&ids);
	printf("%lu\n",iset_start(&ids, 1200003));
	iset_dump(&ids);
	printf("%lu\n",iset_start(&ids, 1200003));
	iset_dump(&ids);

	long arr[]={1000002,1099999,1100000,0,2000006,2000007,2000008,0,9999999,10000000,0};
	for (int i = 0; i < sizeof arr/sizeof arr[0]; ++i) {
		if (arr[i] == 0) {
			printf("\n"); continue;
		}
		printf("%lu: %d ", arr[i], iset_has(&ids, arr[i])); 
	}

	iset_free(&ids);
	return 0;
}
#endif // ISET_UNIT_TEST
