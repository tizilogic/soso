#include "soso.h"


// xoshiro256** 1.0

static inline uint64_t rotl(const uint64_t x, int k) {
	return (x << k) | (x >> (64 - k));
}

static uint64_t s[4] = {1, 2, 3, 4};

uint64_t next(void) {
	const uint64_t result = rotl(s[1] * 5, 7) * 9;

	const uint64_t t = s[1] << 17;

	s[2] ^= s[0];
	s[3] ^= s[1];
	s[1] ^= s[2];
	s[0] ^= s[3];

	s[2] ^= t;

	s[3] = rotl(s[3], 45);

	return result;
}

void soso_random_seed(uint64_t seed) {
	s[1] = 1;
	s[2] = 2;
	s[3] = 3;
	s[0] = seed;
	s[1] = next();
	s[2] = next();
	s[3] = next();
}

uint64_t soso_random_get(void) {
	return next();
}