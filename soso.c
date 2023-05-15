#include "soso.h"

//--- Internals ---//

uint64_t soso_internal_random_get_in(uint64_t min, uint64_t max);
soso_int_t soso_internal_make_card(soso_int_t suit, soso_int_t value);
soso_int_t soso_internal_get_waste_card(const soso_game_t *game);
soso_int_t soso_internal_cvalue(soso_int_t card);
soso_int_t soso_internal_csuit(soso_int_t card);
soso_int_t soso_internal_ccolor(soso_int_t card);
bool soso_internal_valid_card(soso_int_t card);
bool soso_internal_foundation_valid(soso_int_t card, const soso_int_t *foundation_top);
bool soso_internal_tableau_valid(soso_int_t from, soso_int_t to);
int soso_internal_max_draws(const soso_ctx_t *ctx, const soso_game_t *game);
bool soso_internal_game_solved(const soso_game_t *game);
void soso_internal_revert_to_last_move(soso_ctx_t *ctx, soso_game_t *game);
uint32_t soso_internal_state_hash(const soso_game_t *game, const soso_move_t *move);
void soso_internal_add_move(soso_ctx_t *ctx, const soso_move_t m, bool is_auto);
void soso_internal_update_waste_moves(soso_ctx_t *ctx, const soso_game_t *game);
void soso_internal_update_tableau_moves(soso_ctx_t *ctx, const soso_game_t *game);
void soso_internal_update_stock_moves(soso_ctx_t *ctx, const soso_game_t *game);
void soso_internal_update_foundation_moves(soso_ctx_t *ctx, const soso_game_t *game);
bool soso_internal_update_available_moves(soso_ctx_t *ctx, const soso_game_t *game, bool no_stock);
bool soso_internal_draw(soso_game_t *game, int draw_count);
bool soso_internal_undo_draw(soso_game_t *game, int draw_count);
void soso_internal_make_move(soso_ctx_t *ctx, soso_game_t *game, soso_move_t m);
void soso_internal_undo_move(soso_ctx_t *ctx, soso_game_t *game);


//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the
// public domain. The author hereby disclaims copyright to this source
// code.

#ifndef _MURMURHASH3_H_
#define _MURMURHASH3_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------------------

void MurmurHash3_x86_32 (const void *key, int len, uint32_t seed, void *out);

void MurmurHash3_x86_128(const void *key, int len, uint32_t seed, void *out);

void MurmurHash3_x64_128(const void *key, int len, uint32_t seed, void *out);

//-----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif // _MURMURHASH3_H_


//-----------------------------------------------------------------------------
// simplehashtable was written by Tiziano Bettio, and is placed in the
// public domain. The author hereby disclaims copyright to this source
// code.



#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sht sht_t;

/**
 * @brief Initialize a new hashtable with custom allocator.
 *
 * @param item_size The size in bytes of an item
 * @param reserve Number of slots to reserve upon initialization (will be
 * rounded up to the next power of two)
 * @param seed Seed used by the hash function
 * @param custom_alloc Pointer to the allocation function
 * @param custom_free Pointer to the function for freeing allocated memory
 * @return sht_t*
 */
sht_t *sht_init_alloc(uint32_t item_size, uint32_t reserve, uint32_t seed,
                      void *(*custom_alloc)(size_t),
                      void (*custom_free)(void *));

/**
 * @brief Initialize a new hashtable using the memory allocator from `stdlib.h`
 *
 * @param item_size The size in bytes of an item
 * @param reserve Number of slots to reserve upon initialization (will be
 * rounded up to the next power of two)
 * @param seed Seed used by the hash function
 * @return sht_t*
 */
sht_t *sht_init(uint32_t item_size, uint32_t reserve, uint32_t seed);

void sht_destroy(sht_t *sht);

/**
 * @brief Add or overwrite an item in the hashtable.
 *
 * @param sht Pointer to the hashtable
 * @param key Pointer to the key to be used
 * @param len The length in bytes of the passed key
 * @param item Pointer to the item
 * @return Hash value of the entry
 */
uint32_t sht_set(sht_t *sht, const void *key, int len, const void *item);

/**
 * @brief Get an item if present.
 *
 * @param sht Pointer to the hashtable
 * @param key Pointer to the key to be used
 * @param len The length in bytes of the passed key
 * @return void* of the requested item or NULL if not found
 */
void *sht_get(sht_t *sht, const void *key, int len);

/**
 * @brief Get an item by its hash value if present.
 *
 * @param sht Pointer to the hashtable
 * @param hash Hash value of the item
 * @return void* of the requested item or NULL if not found
 */
void *sht_get_by_hash(sht_t *sht, uint32_t hash);

/**
 * @brief Delete an item if present
 *
 * @param sht Pointer to the hashtable
 * @param key Pointer to the key to be used
 * @param len The length in bytes of the passed key
 */
void sht_del(sht_t *sht, const void *key, int len);

/**
 * @brief Number of items stored in the hashtable
 *
 * @param sht Pointer to the hashtable
 * @return uint32_t
 */
uint32_t sht_size(sht_t *sht);

typedef struct sht_it sht_it_t;

/**
 * @brief Get a new iterator for the hashtable
 *
 * @param sht Pointer to the hashtable
 * @return sht_it_t*
 */
sht_it_t *sht_iter(sht_t *sht);

/**
 * @brief Get next item of the iterator.
 *
 * @param it Pointer to the iterator
 * @return void* of the next item or `NULL`
 */
void *sht_iter_next(sht_it_t *it);

void sht_iter_destroy(sht_it_t *it);

/**
 * @brief foreach macro to iterate over a hashtable that expects a pointer to
 * the hashtable, a non initialized pointer to an iterator and a void * (or
 * pointer to the item type).
 *
 */
#define sht_foreach(sht, it, item)                                             \
	for ((it) = sht_iter((sht)), (item) = sht_iter_next((it)); (item) != NULL; \
	     (item) = sht_iter_next((it)))

#ifdef __cplusplus
}
#endif



//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

// Note - The x86 and x64 versions do _not_ produce the same results, as the
// algorithms are optimized for their respective platforms. You can still
// compile and run any of them on any platform, but your performance with the
// non-native version will be less than optimal.



//-----------------------------------------------------------------------------
// Platform-specific functions and macros

#ifdef __GNUC__
#define FORCE_INLINE __attribute__((always_inline)) inline
#else
#define FORCE_INLINE inline
#endif

static FORCE_INLINE uint32_t rotl32 ( uint32_t x, int8_t r )
{
  return (x << r) | (x >> (32 - r));
}

static FORCE_INLINE uint64_t rotl64 ( uint64_t x, int8_t r )
{
  return (x << r) | (x >> (64 - r));
}

#define	ROTL32(x,y)	rotl32(x,y)
#define ROTL64(x,y)	rotl64(x,y)

#define BIG_CONSTANT(x) (x##LLU)

//-----------------------------------------------------------------------------
// Block read - if your platform needs to do endian-swapping or can only
// handle aligned reads, do the conversion here

#define getblock(p, i) (p[i])

//-----------------------------------------------------------------------------
// Finalization mix - force all bits of a hash block to avalanche

static FORCE_INLINE uint32_t fmix32 ( uint32_t h )
{
  h ^= h >> 16;
  h *= 0x85ebca6b;
  h ^= h >> 13;
  h *= 0xc2b2ae35;
  h ^= h >> 16;

  return h;
}

//----------

static FORCE_INLINE uint64_t fmix64 ( uint64_t k )
{
  k ^= k >> 33;
  k *= BIG_CONSTANT(0xff51afd7ed558ccd);
  k ^= k >> 33;
  k *= BIG_CONSTANT(0xc4ceb9fe1a85ec53);
  k ^= k >> 33;

  return k;
}

//-----------------------------------------------------------------------------

void MurmurHash3_x86_32 ( const void * key, int len,
                          uint32_t seed, void * out )
{
  const uint8_t * data = (const uint8_t*)key;
  const int nblocks = len / 4;
  int i;

  uint32_t h1 = seed;

  uint32_t c1 = 0xcc9e2d51;
  uint32_t c2 = 0x1b873593;

  //----------
  // body

  const uint32_t * blocks = (const uint32_t *)(data + nblocks*4);

  for(i = -nblocks; i; i++)
  {
    uint32_t k1 = getblock(blocks,i);

    k1 *= c1;
    k1 = ROTL32(k1,15);
    k1 *= c2;
    
    h1 ^= k1;
    h1 = ROTL32(h1,13); 
    h1 = h1*5+0xe6546b64;
  }

  //----------
  // tail

  const uint8_t * tail = (const uint8_t*)(data + nblocks*4);

  uint32_t k1 = 0;

  switch(len & 3)
  {
  case 3: k1 ^= tail[2] << 16;
  case 2: k1 ^= tail[1] << 8;
  case 1: k1 ^= tail[0];
          k1 *= c1; k1 = ROTL32(k1,15); k1 *= c2; h1 ^= k1;
  };

  //----------
  // finalization

  h1 ^= len;

  h1 = fmix32(h1);

  *(uint32_t*)out = h1;
} 

//-----------------------------------------------------------------------------

void MurmurHash3_x86_128 ( const void * key, const int len,
                           uint32_t seed, void * out )
{
  const uint8_t * data = (const uint8_t*)key;
  const int nblocks = len / 16;
  int i;

  uint32_t h1 = seed;
  uint32_t h2 = seed;
  uint32_t h3 = seed;
  uint32_t h4 = seed;

  uint32_t c1 = 0x239b961b; 
  uint32_t c2 = 0xab0e9789;
  uint32_t c3 = 0x38b34ae5; 
  uint32_t c4 = 0xa1e38b93;

  //----------
  // body

  const uint32_t * blocks = (const uint32_t *)(data + nblocks*16);

  for(i = -nblocks; i; i++)
  {
    uint32_t k1 = getblock(blocks,i*4+0);
    uint32_t k2 = getblock(blocks,i*4+1);
    uint32_t k3 = getblock(blocks,i*4+2);
    uint32_t k4 = getblock(blocks,i*4+3);

    k1 *= c1; k1  = ROTL32(k1,15); k1 *= c2; h1 ^= k1;

    h1 = ROTL32(h1,19); h1 += h2; h1 = h1*5+0x561ccd1b;

    k2 *= c2; k2  = ROTL32(k2,16); k2 *= c3; h2 ^= k2;

    h2 = ROTL32(h2,17); h2 += h3; h2 = h2*5+0x0bcaa747;

    k3 *= c3; k3  = ROTL32(k3,17); k3 *= c4; h3 ^= k3;

    h3 = ROTL32(h3,15); h3 += h4; h3 = h3*5+0x96cd1c35;

    k4 *= c4; k4  = ROTL32(k4,18); k4 *= c1; h4 ^= k4;

    h4 = ROTL32(h4,13); h4 += h1; h4 = h4*5+0x32ac3b17;
  }

  //----------
  // tail

  const uint8_t * tail = (const uint8_t*)(data + nblocks*16);

  uint32_t k1 = 0;
  uint32_t k2 = 0;
  uint32_t k3 = 0;
  uint32_t k4 = 0;

  switch(len & 15)
  {
  case 15: k4 ^= tail[14] << 16;
  case 14: k4 ^= tail[13] << 8;
  case 13: k4 ^= tail[12] << 0;
           k4 *= c4; k4  = ROTL32(k4,18); k4 *= c1; h4 ^= k4;

  case 12: k3 ^= tail[11] << 24;
  case 11: k3 ^= tail[10] << 16;
  case 10: k3 ^= tail[ 9] << 8;
  case  9: k3 ^= tail[ 8] << 0;
           k3 *= c3; k3  = ROTL32(k3,17); k3 *= c4; h3 ^= k3;

  case  8: k2 ^= tail[ 7] << 24;
  case  7: k2 ^= tail[ 6] << 16;
  case  6: k2 ^= tail[ 5] << 8;
  case  5: k2 ^= tail[ 4] << 0;
           k2 *= c2; k2  = ROTL32(k2,16); k2 *= c3; h2 ^= k2;

  case  4: k1 ^= tail[ 3] << 24;
  case  3: k1 ^= tail[ 2] << 16;
  case  2: k1 ^= tail[ 1] << 8;
  case  1: k1 ^= tail[ 0] << 0;
           k1 *= c1; k1  = ROTL32(k1,15); k1 *= c2; h1 ^= k1;
  };

  //----------
  // finalization

  h1 ^= len; h2 ^= len; h3 ^= len; h4 ^= len;

  h1 += h2; h1 += h3; h1 += h4;
  h2 += h1; h3 += h1; h4 += h1;

  h1 = fmix32(h1);
  h2 = fmix32(h2);
  h3 = fmix32(h3);
  h4 = fmix32(h4);

  h1 += h2; h1 += h3; h1 += h4;
  h2 += h1; h3 += h1; h4 += h1;

  ((uint32_t*)out)[0] = h1;
  ((uint32_t*)out)[1] = h2;
  ((uint32_t*)out)[2] = h3;
  ((uint32_t*)out)[3] = h4;
}

//-----------------------------------------------------------------------------

void MurmurHash3_x64_128 ( const void * key, const int len,
                           const uint32_t seed, void * out )
{
  const uint8_t * data = (const uint8_t*)key;
  const int nblocks = len / 16;
  int i;

  uint64_t h1 = seed;
  uint64_t h2 = seed;

  uint64_t c1 = BIG_CONSTANT(0x87c37b91114253d5);
  uint64_t c2 = BIG_CONSTANT(0x4cf5ad432745937f);

  //----------
  // body

  const uint64_t * blocks = (const uint64_t *)(data);

  for(i = 0; i < nblocks; i++)
  {
    uint64_t k1 = getblock(blocks,i*2+0);
    uint64_t k2 = getblock(blocks,i*2+1);

    k1 *= c1; k1  = ROTL64(k1,31); k1 *= c2; h1 ^= k1;

    h1 = ROTL64(h1,27); h1 += h2; h1 = h1*5+0x52dce729;

    k2 *= c2; k2  = ROTL64(k2,33); k2 *= c1; h2 ^= k2;

    h2 = ROTL64(h2,31); h2 += h1; h2 = h2*5+0x38495ab5;
  }

  //----------
  // tail

  const uint8_t * tail = (const uint8_t*)(data + nblocks*16);

  uint64_t k1 = 0;
  uint64_t k2 = 0;

  switch(len & 15)
  {
  case 15: k2 ^= (uint64_t)(tail[14]) << 48;
  case 14: k2 ^= (uint64_t)(tail[13]) << 40;
  case 13: k2 ^= (uint64_t)(tail[12]) << 32;
  case 12: k2 ^= (uint64_t)(tail[11]) << 24;
  case 11: k2 ^= (uint64_t)(tail[10]) << 16;
  case 10: k2 ^= (uint64_t)(tail[ 9]) << 8;
  case  9: k2 ^= (uint64_t)(tail[ 8]) << 0;
           k2 *= c2; k2  = ROTL64(k2,33); k2 *= c1; h2 ^= k2;

  case  8: k1 ^= (uint64_t)(tail[ 7]) << 56;
  case  7: k1 ^= (uint64_t)(tail[ 6]) << 48;
  case  6: k1 ^= (uint64_t)(tail[ 5]) << 40;
  case  5: k1 ^= (uint64_t)(tail[ 4]) << 32;
  case  4: k1 ^= (uint64_t)(tail[ 3]) << 24;
  case  3: k1 ^= (uint64_t)(tail[ 2]) << 16;
  case  2: k1 ^= (uint64_t)(tail[ 1]) << 8;
  case  1: k1 ^= (uint64_t)(tail[ 0]) << 0;
           k1 *= c1; k1  = ROTL64(k1,31); k1 *= c2; h1 ^= k1;
  };

  //----------
  // finalization

  h1 ^= len; h2 ^= len;

  h1 += h2;
  h2 += h1;

  h1 = fmix64(h1);
  h2 = fmix64(h2);

  h1 += h2;
  h2 += h1;

  ((uint64_t*)out)[0] = h1;
  ((uint64_t*)out)[1] = h2;
}

//-----------------------------------------------------------------------------





#include <assert.h>
#include <string.h>

struct sht {
	uint8_t *table;
	uint32_t item_size;
	uint32_t size;
	uint32_t capacity;
	uint32_t seed;
	void *(*myalloc)(size_t);
	void (*myfree)(void *);
};

static uint32_t next_power_of_two(uint32_t v) {
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;
	return v;
}

sht_t *sht_init_alloc(uint32_t item_size, uint32_t reserve, uint32_t seed,
                      void *(*custom_alloc)(size_t),
                      void (*custom_free)(void *)) {
	sht_t *sht = (sht_t *)custom_alloc(sizeof(sht_t));
	assert(sht != NULL);
	sht->myalloc = custom_alloc;
	sht->myfree = custom_free;
	// ensure 4 byte alignment and reserve additional space for hash value
	sht->item_size = item_size + (item_size % 4) + sizeof(uint32_t);
	sht->seed = seed;
	sht->size = 0;
	sht->capacity = next_power_of_two(reserve);
	if (sht->capacity < 2) sht->capacity = 2;
	sht->table = (uint8_t *)custom_alloc(sht->capacity * sht->item_size);
	assert(sht->table != NULL);
	memset(sht->table, 0, sht->capacity * sht->item_size);
	return sht;
}

sht_t *sht_init(uint32_t item_size, uint32_t reserve, uint32_t seed) {
	return sht_init_alloc(item_size, reserve, seed, malloc, free);
}

void sht_destroy(sht_t *sht) {
	assert(sht != NULL);
	if (sht->table != NULL) sht->myfree(sht->table);
	sht->myfree(sht);
}

static uint32_t insert(uint8_t *table, uint32_t k, uint32_t cap, const void *el,
                       uint32_t size) {
	uint32_t id = k & (cap - 1);
	for (;;) {
		uint32_t cmp = *(uint32_t *)&table[id * size];
		if (cmp > 0 && cmp != k) {
			++id;
			id = id % cap;
			continue;
		}
		memcpy(&table[id * size], &k, sizeof(uint32_t));
		memcpy(&table[id * size + sizeof(uint32_t)], el,
		       size - sizeof(uint32_t));
		if (cmp == 0) return 1;
		return 0;
	}
}

static void grow(sht_t *sht) {
	if ((sht->size << 1) < sht->capacity) return;
	sht_t tmp;
	memcpy(&tmp, sht, sizeof(sht_t));
	tmp.capacity = sht->capacity << 1;
	tmp.table = (uint8_t *)sht->myalloc(tmp.capacity * sht->item_size);
	assert(tmp.table != NULL);
	memset(tmp.table, 0, tmp.capacity * tmp.item_size);
	for (uint32_t i = 0; i < sht->capacity; ++i) {
		uint32_t *k = (uint32_t *)&sht->table[i * sht->item_size];
		if (*k == 0) continue;
		insert(tmp.table, *k, tmp.capacity,
		       &sht->table[i * tmp.item_size + sizeof(uint32_t)],
		       tmp.item_size);
	}
	sht->myfree(sht->table);
	memcpy(sht, &tmp, sizeof(sht_t));
}

static uint32_t comp_key(const void *key, int len, uint32_t seed) {
	uint32_t k;
	MurmurHash3_x86_32(key, len, seed, &k);
	if (k == 0) k = 1;
	return k;
}

uint32_t sht_set(sht_t *sht, const void *key, int len, const void *element) {
	assert(sht != NULL);
	grow(sht);
	uint32_t hash = comp_key(key, len, sht->seed);
	if (insert(sht->table, hash, sht->capacity, element, sht->item_size) > 0)
		++(sht->size);
	return hash;
}

void *sht_get(sht_t *sht, const void *key, int len) {
	assert(sht != NULL);
	uint32_t k = comp_key(key, len, sht->seed);
	return sht_get_by_hash(sht, k);
}

void *sht_get_by_hash(sht_t *sht, uint32_t hash) {
	assert(sht != NULL);
	uint32_t id = hash & (sht->capacity - 1);
	for (;;) {
		uint32_t cmp = *(uint32_t *)&sht->table[id * sht->item_size];
		if (cmp == 0) return NULL;
		if (cmp == hash) break;
		++id;
		id = id % sht->capacity;
	}
	return (void *)&sht->table[id * sht->item_size + sizeof(uint32_t)];
}

static void move_left(sht_t *sht, uint32_t start) {
	uint32_t k = *(uint32_t *)&sht->table[start * sht->item_size];
	if (k == 0) return;
	uint32_t id = k & (sht->capacity - 1);
	if (id == start) return;
	uint32_t dest = (start == 0) ? sht->capacity - 1 : start - 1;
	memcpy(&sht->table[dest * sht->item_size],
	       &sht->table[start * sht->item_size], sht->item_size);
	memset(&sht->table[start * sht->item_size], 0, sht->item_size);
	move_left(sht, (start + 1) % sht->capacity);
}

void sht_del(sht_t *sht, const void *key, int len) {
	assert(sht != NULL);
	uint32_t k = comp_key(key, len, sht->seed);
	uint32_t id = k & (sht->capacity - 1);
	for (;;) {
		uint32_t cmp = *(uint32_t *)&sht->table[id * sht->item_size];
		if (cmp == 0) return;
		if (cmp == k) break;
		++id;
		id = id % sht->capacity;
	}
	--(sht->size);
	memset(&sht->table[id * sht->item_size], 0, sht->item_size);

	move_left(sht, (id + 1) % sht->capacity);
}

uint32_t sht_size(sht_t *sht) {
	assert(sht != NULL);
	return sht->size;
}

struct sht_it {
	sht_t *sht;
	uint32_t cur;
	int valid;
};

sht_it_t *sht_iter(sht_t *sht) {
	assert(sht != NULL);
	sht_it_t *it = (sht_it_t *)sht->myalloc(sizeof(sht_it_t));
	assert(it != NULL);
	it->sht = sht;
	it->cur = 0;
	it->valid = 1;
	return it;
}

void *sht_iter_next(sht_it_t *it) {
	assert(it != NULL && it->valid > 0);
	sht_t *sht = it->sht;
	for (; it->cur < sht->capacity;) {
		uint32_t cmp = *(uint32_t *)&sht->table[it->cur * sht->item_size];
		++(it->cur);
		if (cmp != 0)
			return (void *)&sht
			    ->table[(it->cur - 1) * sht->item_size + sizeof(uint32_t)];
	}
	it->valid = 0;
	return NULL;
}

void sht_iter_destroy(sht_it_t *it) {
	assert(it != NULL);
	it->sht->myfree(it);
}





// xoshiro256** 1.0

static inline uint64_t rotl(const uint64_t x, int k) {
	return (x << k) | (x >> (64 - k));
}

static uint64_t s[4] = {1, 2, 3, 4};

static uint64_t next(void) {
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





#include <string.h>

#define SOSO_NUM_SHUFFLES 12
#define SOSO_HASH_SEED 0xdeadc0de
const char *soso_suits = "CDSH";
const char *soso_values = "A23456789TJQK";

void soso_shuffle(soso_deck_t *deck, uint64_t seed) {
	soso_random_seed(seed);
	for (soso_int_t i = 0; i < 4; ++i)
		for (soso_int_t j = 0; j < 13; ++j) {
			deck->cards[i * 13 + j] = soso_internal_make_card(i, j);
		}

	for (int s = 0; s < SOSO_NUM_SHUFFLES; ++s)
		for (int i = 0; i < 51; ++i) {
			int j = soso_internal_random_get_in(i, 51);
			if (i == j) continue;
			soso_int_t t = deck->cards[j];
			deck->cards[j] = deck->cards[i];
			deck->cards[i] = t;
		}
}

void soso_deal(soso_game_t *game, soso_deck_t *deck) {
	memset(game, SOSO_EMPTY_CARD, sizeof(soso_game_t));
	game->stock_cur = 24;
	game->stock_count = 24;
	for (int i = 0; i < 7; ++i) {
		game->tableau_top[i] = i + 1;
		game->tableau_up[i] = i;
		if (i < 4) game->foundation_top[i] = 0;
	}

	int next = 51;
	for (int i = 0; i < 7; ++i)
		for (int j = i; j < 7; ++j) game->tableau[j][i] = deck->cards[next--];
	int stock_id = 23;
	while (next > -1) game->stock[stock_id--] = deck->cards[next--];
}

void soso_export(const soso_game_t *game, char *buffer) {
	int cur = 0;
	for (int i = 0; i < SOSO_TOTAL_PILES; ++i) {
		switch (i) {
		// Treat the empty pile as waste
		case SOSO_EMPTY_PILE: {
			buffer[cur++] = ' ';
			buffer[cur++] = 'W';
			buffer[cur++] = ':';
			buffer[cur++] = ' ';
			if (game->stock_cur < game->stock_count)
				for (int j = game->stock_cur; j < game->stock_count; ++j) {
					buffer[cur++] = soso_values[soso_internal_cvalue(game->stock[j])];
					buffer[cur++] = soso_suits[soso_internal_csuit(game->stock[j])];
					buffer[cur++] = ' ';
				}
		} break;
		case SOSO_STOCK_WASTE: {
			buffer[cur++] = ' ';
			buffer[cur++] = 'S';
			buffer[cur++] = ':';
			buffer[cur++] = ' ';
			if (game->stock_cur > 0)
				for (int j = game->stock_cur - 1; j >= 0; --j) {
					buffer[cur++] = soso_values[soso_internal_cvalue(game->stock[j])];
					buffer[cur++] = soso_suits[soso_internal_csuit(game->stock[j])];
					buffer[cur++] = ' ';
				}
		} break;
		case SOSO_TABLEAU1:
		case SOSO_TABLEAU2:
		case SOSO_TABLEAU3:
		case SOSO_TABLEAU4:
		case SOSO_TABLEAU5:
		case SOSO_TABLEAU6:
		case SOSO_TABLEAU7: {
			int t = i - SOSO_TABLEAU1;
			buffer[cur++] = 'T';
			buffer[cur++] = 49 + t;
			buffer[cur++] = ':';
			buffer[cur++] = ' ';
			if (game->tableau_top[t] > 0) {
				for (int j = game->tableau_top[t] - 1; j >= game->tableau_up[t]; --j) {
					buffer[cur++] = soso_values[soso_internal_cvalue(game->tableau[t][j])];
					buffer[cur++] = soso_suits[soso_internal_csuit(game->tableau[t][j])];
					buffer[cur++] = ' ';
				}
				buffer[cur++] = '|';
				buffer[cur++] = ' ';
				for (int j = game->tableau_up[t] - 1; j >= 0; --j) {
					buffer[cur++] = soso_values[soso_internal_cvalue(game->tableau[t][j])];
					buffer[cur++] = soso_suits[soso_internal_csuit(game->tableau[t][j])];
					buffer[cur++] = ' ';
				}
			}
		} break;
		case SOSO_FOUNDATION1C:
		case SOSO_FOUNDATION2D:
		case SOSO_FOUNDATION3S:
		case SOSO_FOUNDATION4H: {
			int f = i - SOSO_FOUNDATION1C;
			buffer[cur++] = 'F';
			buffer[cur++] = soso_suits[f];
			buffer[cur++] = ':';
			buffer[cur++] = ' ';
			for (int j = 0; j < game->foundation_top[f]; ++j) {
				buffer[cur++] = soso_suits[soso_internal_csuit(f)];
				buffer[cur++] = soso_values[soso_internal_cvalue(j)];
				buffer[cur++] = ' ';
			}
		} break;
		}
		buffer[cur++] = '\n';
	}
	buffer[cur++] = '\0';
}

void soso_ctx_init(soso_ctx_t *ctx, int draw_count, int max_visited, void *(*custom_alloc)(size_t),
                   void *(*custom_realloc)(void *, size_t), void (*custom_free)(void *)) {
	ctx->draw_count = draw_count;
	ctx->max_visited = max_visited;
	ctx->visited = NULL;
	ctx->moves = NULL;
	ctx->moves_cap = 0;
	ctx->moves_top = 0;
	ctx->moves_available_top = 0;
	ctx->automoves_count = 0;
	ctx->moves_total = 0;
	if (custom_alloc != NULL) {
		ctx->alloc = custom_alloc;
		ctx->realloc = custom_realloc;
		ctx->free = custom_free;
	}
	else {
		ctx->alloc = malloc;
		ctx->realloc = realloc;
		ctx->free = free;
	}
}

void soso_ctx_destroy(soso_ctx_t *ctx) {
	if (ctx->moves != NULL) ctx->free(ctx->moves);
	if (ctx->visited != NULL) sht_destroy(ctx->visited);
}

static void grow_moves(soso_ctx_t *ctx) {
	if (ctx->moves_top >= ctx->moves_cap) {
		if (ctx->moves_cap == 0) {
			ctx->moves = (soso_move_t *)ctx->alloc(sizeof(soso_move_t) * 2);
			ctx->moves_cap = 2;
		}
		else {
			ctx->moves =
			    (soso_move_t *)ctx->realloc(ctx->moves, sizeof(soso_move_t) * ctx->moves_cap * 2);
			ctx->moves_cap *= 2;
		}
		assert(ctx->moves != NULL);
	}
}

void soso_internal_add_move(soso_ctx_t *ctx, const soso_move_t m, bool is_auto) {
	grow_moves(ctx);
	ctx->moves[ctx->moves_top++] = m;
	++ctx->moves_total;
	if (is_auto) ++ctx->automoves_count;
}

void soso_internal_update_waste_moves(soso_ctx_t *ctx, const soso_game_t *game) {
	soso_int_t card_from = soso_internal_get_waste_card(game);
	if (card_from == -1) return;

	// Waste->Foundation
	if (soso_internal_foundation_valid(card_from, game->foundation_top)) {
		ctx->moves_available[ctx->moves_available_top].from = SOSO_STOCK_WASTE;
		ctx->moves_available[ctx->moves_available_top].to =
		    SOSO_FOUNDATION1C + soso_internal_csuit(card_from);
		ctx->moves_available[ctx->moves_available_top].count = 1;
		ctx->moves_available[ctx->moves_available_top++].extra = SOSO_NOEXTRA;
	}
	// Waste->Tableau
	if (soso_internal_cvalue(card_from) < 1) return; // No need to place Ace
	bool king_placed = false;
	for (int i = 0; i < 7; ++i) {
		if (!king_placed && game->tableau_top[i] == 0 &&
		    soso_internal_cvalue(card_from) == SOSO_KING) {
			king_placed = true;
			ctx->moves_available[ctx->moves_available_top].from = SOSO_STOCK_WASTE;
			ctx->moves_available[ctx->moves_available_top].to = SOSO_TABLEAU1 + i;
			ctx->moves_available[ctx->moves_available_top].count = 1;
			ctx->moves_available[ctx->moves_available_top++].extra = SOSO_NOEXTRA;
			continue;
		}
		if (game->tableau_top[i] == 0) continue;
		soso_int_t card_to = game->tableau[i][game->tableau_top[i] - 1];
		if (soso_internal_tableau_valid(card_from, card_to)) {
			ctx->moves_available[ctx->moves_available_top].from = SOSO_STOCK_WASTE;
			ctx->moves_available[ctx->moves_available_top].to = SOSO_TABLEAU1 + i;
			ctx->moves_available[ctx->moves_available_top].count = 1;
			ctx->moves_available[ctx->moves_available_top++].extra = SOSO_NOEXTRA;
		}
	}
}

void soso_internal_update_tableau_moves(soso_ctx_t *ctx, const soso_game_t *game) {
	soso_int_t card_from, card_to;
	for (int i = 0; i < 7; ++i) {
		if (game->tableau_top[i] < 1) continue;
		card_from = game->tableau[i][game->tableau_top[i] - 1];
		// Tableau->Foundation
		if (game->tableau_top[i] > 0 &&
		    soso_internal_foundation_valid(card_from, game->foundation_top)) {
			ctx->moves_available[ctx->moves_available_top].from = SOSO_TABLEAU1 + i;
			ctx->moves_available[ctx->moves_available_top].to =
			    SOSO_FOUNDATION1C + soso_internal_csuit(card_from);
			ctx->moves_available[ctx->moves_available_top].count = 1;
			ctx->moves_available[ctx->moves_available_top++].extra = SOSO_NOEXTRA;
		}

		// Tableau->Tableau top card
		if (game->tableau_top[i] - game->tableau_up[i] != 1) continue;
		bool king_placed = false;
		for (int j = 0; j < 7; ++j) {
			if (i == j) continue;
			if (!king_placed && game->tableau_top[j] == 0 &&
			    soso_internal_cvalue(card_from) == SOSO_KING) {
				king_placed = true;
				ctx->moves_available[ctx->moves_available_top].from = SOSO_TABLEAU1 + i;
				ctx->moves_available[ctx->moves_available_top].to = SOSO_TABLEAU1 + j;
				ctx->moves_available[ctx->moves_available_top].count = 1;
				ctx->moves_available[ctx->moves_available_top++].extra = SOSO_NOEXTRA;
				continue;
			}

			card_to = game->tableau[j][game->tableau_top[j] - 1];
			if (soso_internal_tableau_valid(card_from, card_to)) {
				ctx->moves_available[ctx->moves_available_top].from = SOSO_TABLEAU1 + i;
				ctx->moves_available[ctx->moves_available_top].to = SOSO_TABLEAU1 + j;
				ctx->moves_available[ctx->moves_available_top].count = 1;
				ctx->moves_available[ctx->moves_available_top++].extra = SOSO_NOEXTRA;
			}
		}
	}

	// Tableau talon swap
	for (int i = 0; i < 7; ++i) {
		soso_int_t talonsize = game->tableau_top[i] - game->tableau_up[i];
		if (game->tableau_top[i] == 0 || talonsize == 1) continue;

		bool cards_closed = game->tableau_up[i] > 0;
		bool king_first = soso_internal_cvalue(game->tableau[i][game->tableau_up[i]]) == SOSO_KING;
		int start = (!cards_closed && king_first) ? 1 : 0;

		bool king_placed = false;
		for (int j = 0; j < 7; ++j) {
			if (i == j) continue;

			// King talon
			if (!king_placed && game->tableau_top[j] == 0 && king_first &&
			    game->tableau_up[i] > 0) {
				king_placed = true;
				ctx->moves_available[ctx->moves_available_top].from = SOSO_TABLEAU1 + i;
				ctx->moves_available[ctx->moves_available_top].to = SOSO_TABLEAU1 + j;
				ctx->moves_available[ctx->moves_available_top].count = talonsize;
				ctx->moves_available[ctx->moves_available_top++].extra = SOSO_NOEXTRA;
			}

			if (game->tableau_top[j] == 0) continue;

			for (int offset = start; offset < talonsize; ++offset) {
				if (offset > 0) {
					card_from = game->tableau[i][game->tableau_up[i] + offset - 1];
					if (game->foundation_top[soso_internal_csuit(card_from)] !=
					    soso_internal_cvalue(card_from))
						continue;
				}
				card_from = game->tableau[i][game->tableau_up[i] + offset];
				card_to = game->tableau[j][game->tableau_top[j] - 1];
				if (soso_internal_tableau_valid(card_from, card_to)) {
					ctx->moves_available[ctx->moves_available_top].from = SOSO_TABLEAU1 + i;
					ctx->moves_available[ctx->moves_available_top].to = SOSO_TABLEAU1 + j;
					ctx->moves_available[ctx->moves_available_top].count = talonsize - offset;
					ctx->moves_available[ctx->moves_available_top++].extra = SOSO_NOEXTRA;
				}
			}
		}
	}
}

void soso_internal_update_stock_moves(soso_ctx_t *ctx, const soso_game_t *game) {
	if (game->stock_count == 0) return;

	soso_int_t current_cursor = game->stock_cur;
	soso_ctx_t pseudo_ctx = (soso_ctx_t){.moves_available_top = 0, .draw_count = ctx->draw_count};
	soso_game_t game_copy;
	memcpy(&game_copy, game, sizeof(soso_game_t));
	int numdraw = 0;
	int maxdraw = soso_internal_max_draws(ctx, game);
	for (int i = 0; i < maxdraw; ++i) {
		if (!soso_internal_draw(&game_copy, ctx->draw_count)) break;
		++numdraw;
		soso_internal_update_waste_moves(&pseudo_ctx, &game_copy);
		if (pseudo_ctx.moves_available_top > 0) {
			ctx->moves_available[ctx->moves_available_top].from = SOSO_STOCK_WASTE;
			ctx->moves_available[ctx->moves_available_top].to = SOSO_STOCK_WASTE;
			ctx->moves_available[ctx->moves_available_top].count = numdraw;
			ctx->moves_available[ctx->moves_available_top++].extra = SOSO_NOEXTRA;
			break;
		}
	}
}

void soso_internal_update_foundation_moves(soso_ctx_t *ctx, const soso_game_t *game) {
	for (int i = 0; i < 4; ++i) {
		if (game->foundation_top[i] > 2) {
			bool king_placed = false;
			for (int j = 0; j < 7; ++j) {
				if (soso_internal_tableau_valid(
				        soso_internal_make_card(i, game->foundation_top[i] - 1),
				        game->tableau[j][game->tableau_top[j] - 1])) {
					ctx->moves_available[ctx->moves_available_top].from = SOSO_FOUNDATION1C + i;
					ctx->moves_available[ctx->moves_available_top].to = SOSO_TABLEAU1 + j;
					ctx->moves_available[ctx->moves_available_top].count = 1;
					ctx->moves_available[ctx->moves_available_top++].extra = SOSO_NOEXTRA;
				}
				else if (!king_placed && game->foundation_top[i] == 13 &&
				         game->tableau_top[j] == 0) {
					king_placed = true;
					ctx->moves_available[ctx->moves_available_top].from = SOSO_FOUNDATION1C + i;
					ctx->moves_available[ctx->moves_available_top].to = SOSO_TABLEAU1 + j;
					ctx->moves_available[ctx->moves_available_top].count = 1;
					ctx->moves_available[ctx->moves_available_top++].extra = SOSO_NOEXTRA;
				}
			}
		}
	}
}

static int soso_internal_rate_move(soso_move_t a, const soso_game_t *game, const soso_ctx_t *ctx) {
	if (a.from >= SOSO_TABLEAU1 && a.from <= SOSO_TABLEAU7) {
		int from = a.from - SOSO_TABLEAU1;
		if (game->tableau_up[from] > 0 &&
		    game->tableau_up[from] == game->tableau_top[from] - a.count)
			return 2;
		if (a.to >= SOSO_FOUNDATION1C) return 1;
		return 0;
	}
	if (a.to >= SOSO_FOUNDATION1C) return 1;
	if (a.from >= SOSO_FOUNDATION1C) {
		soso_game_t game_copy = *game;
		soso_ctx_t pseudo_ctx = {.moves_available_top = 0, .draw_count = ctx->draw_count};
		soso_internal_make_move(&pseudo_ctx, &game_copy, a);
		soso_internal_update_tableau_moves(&pseudo_ctx, &game_copy);
		soso_internal_update_waste_moves(&pseudo_ctx, &game_copy);
		int best = -1;
		for (int i = 0; i < pseudo_ctx.moves_available_top; ++i) {
			soso_move_t m = pseudo_ctx.moves_available[i];
			if (m.from == a.to && m.to == a.from) continue;
			int rating = soso_internal_rate_move(m, &game_copy, &pseudo_ctx);
			best = (best < rating) ? rating : best;
		}
		return best;
	}
	if (a.to >= SOSO_TABLEAU1 && a.to <= SOSO_TABLEAU7) return 1;
	if (a.from == a.to && a.from == SOSO_STOCK_WASTE) {
		soso_game_t game_copy = *game;
		for (int i = 0; i < a.count; ++i) {
			soso_internal_draw(&game_copy, ctx->draw_count);
		}
		soso_ctx_t pseudo_ctx = {.moves_available_top = 0};
		soso_internal_update_waste_moves(&pseudo_ctx, &game_copy);
		int best = -10;
		for (int i = 0; i < pseudo_ctx.moves_available_top; ++i) {
			int rating = (pseudo_ctx.moves_available[i].to >= SOSO_FOUNDATION1C) ? 1 : 0;
			best = (best < rating) ? rating : best;
		}
		return best;
	}
	return 0;
}

static void soso_internal_sort_available(soso_ctx_t *ctx, const soso_game_t *game) {
	for (int i = 0; i < ctx->moves_available_top - 1; ++i)
		for (int j = i + 1; j < ctx->moves_available_top; ++j)
			if (soso_internal_rate_move(ctx->moves_available[i], game, ctx) <
			    soso_internal_rate_move(ctx->moves_available[j], game, ctx)) {
				soso_move_t t = ctx->moves_available[i];
				ctx->moves_available[i] = ctx->moves_available[j];
				ctx->moves_available[j] = t;
			}
}

bool soso_internal_update_available_moves(soso_ctx_t *ctx, const soso_game_t *game, bool no_stock) {
	ctx->moves_available_top = 0;
	if (!no_stock) soso_internal_update_stock_moves(ctx, game);
	soso_internal_update_tableau_moves(ctx, game);
	soso_internal_update_waste_moves(ctx, game);
	soso_internal_update_foundation_moves(ctx, game);
	soso_internal_sort_available(ctx, game);
	return ctx->moves_available_top > 0;
}

bool soso_internal_draw(soso_game_t *game, int draw_count) {
	if (game->stock_count == 0) return false;
	if (game->stock_cur == 0) {
		game->stock_cur = game->stock_count;
		for (int i = game->stock_count; i < 24; ++i) game->stock[i] = SOSO_EMPTY_CARD;
	}
	else
		game->stock_cur = (game->stock_cur - draw_count > 0) ? game->stock_cur - draw_count : 0;
	return true;
}

bool soso_internal_undo_draw(soso_game_t *game, int draw_count) {
	if (game->stock_count == 0) return false;
	if (game->stock_cur == game->stock_count) {
		game->stock_cur = 0;
	}
	else if (game->stock_cur == 0)
		game->stock_cur =
		    (game->stock_count % draw_count) ? (game->stock_count % draw_count) : draw_count;
	else
		game->stock_cur += draw_count;
	return true;
}

void soso_make_auto_moves(soso_ctx_t *ctx, soso_game_t *game) {
	// Turn flipable cards
	for (int i = 0; i < 7; ++i) {
		if (game->tableau_top[i] > 0 && game->tableau_top[i] == game->tableau_up[i]) {
			--game->tableau_up[i];
			soso_internal_add_move(ctx,
			                       (soso_move_t){.from = i + SOSO_TABLEAU1,
			                                     .to = i + SOSO_TABLEAU1,
			                                     .count = 1,
			                                     .extra = SOSO_FLIP | SOSO_AUTO_MOVE},
			                       true);
		}
	}

	// Optionally draw until a move becomes available
	int numdraw = 0;
	int maxdraw = soso_internal_max_draws(ctx, game);
	for (int i = 0; i < maxdraw; ++i) {
		if (soso_internal_update_available_moves(ctx, game, true)) break;
		if (!soso_internal_draw(game, ctx->draw_count)) break;
		++numdraw;
	}
	if (numdraw > 0 && numdraw < maxdraw)
		soso_internal_add_move(ctx,
		                       (soso_move_t){.count = numdraw,
		                                     .from = SOSO_STOCK_WASTE,
		                                     .to = SOSO_STOCK_WASTE,
		                                     .extra = SOSO_AUTO_MOVE},
		                       true);
}

void soso_internal_undo_move(soso_ctx_t *ctx, soso_game_t *game) {
	assert(ctx->moves_top > 0);
	soso_move_t m = ctx->moves[ctx->moves_top - 1];
	if ((m.extra & SOSO_AUTO_MOVE) > 0) --ctx->automoves_count;

	if (m.from == SOSO_STOCK_WASTE) {
		if (m.to == SOSO_STOCK_WASTE) {
			// Undo Draw
			for (int i = 0; i < m.count; ++i) {
				if (!soso_internal_undo_draw(game, ctx->draw_count)) assert(0);
			}
		}
		else if (m.to >= SOSO_TABLEAU1 && m.to <= SOSO_TABLEAU7) {
			// Undo Waste -> Tableau
			++(game->stock_count);
			for (int i = game->stock_cur + 1; i < game->stock_count; ++i) {
				game->stock[i] = game->stock[i - 1];
			}
			soso_int_t i = m.to - SOSO_TABLEAU1;
			--game->tableau_top[i];
			game->stock[game->stock_cur] = game->tableau[i][game->tableau_top[i]];
		}
		else if (m.to >= SOSO_FOUNDATION1C && m.to <= SOSO_FOUNDATION4H) {
			// Undo Waste -> Foundation
			++(game->stock_count);
			for (int i = game->stock_cur + 1; i < game->stock_count; ++i) {
				game->stock[i] = game->stock[i - 1];
			}
			soso_int_t i = m.to - SOSO_FOUNDATION1C;
			--game->foundation_top[i];
			game->stock[game->stock_cur] = soso_internal_make_card(i, game->foundation_top[i]);
		}
		else {
			assert(0);
		}
	}
	else if (m.from >= SOSO_TABLEAU1 && m.from <= SOSO_TABLEAU7) {
		// Undo Tableau -> ?
		if (m.to == m.from && (m.extra & SOSO_FLIP) > 0) {
			// Undo Flip
			soso_int_t to = m.to - SOSO_TABLEAU1;
			++game->tableau_up[to];
		}
		else if (m.to >= SOSO_TABLEAU1 && m.to <= SOSO_TABLEAU7) {
			// Undo Tableau -> Tableau
			soso_int_t to = m.to - SOSO_TABLEAU1;
			soso_int_t from = m.from - SOSO_TABLEAU1;
			for (int i = 0; i < m.count; ++i) {
				game->tableau[from][game->tableau_top[from]++] =
				    game->tableau[to][(game->tableau_top[to] - m.count) + i];
				game->tableau[to][(game->tableau_top[to] - m.count) + i] = SOSO_EMPTY_CARD;
			}
			game->tableau_top[to] -= m.count;
		}
		else if (m.to >= SOSO_FOUNDATION1C && m.to <= SOSO_FOUNDATION4H) {
			// Undo Tableau -> Foundation
			soso_int_t suit = m.to - SOSO_FOUNDATION1C;
			soso_int_t from = m.from - SOSO_TABLEAU1;
			--game->foundation_top[suit];
			game->tableau[from][game->tableau_top[from]++] =
			    soso_internal_make_card(suit, game->foundation_top[suit]);
		}
		else {
			assert(0);
		}
	}
	else if (m.from >= SOSO_FOUNDATION1C && m.from <= SOSO_FOUNDATION4H) {
		assert(m.to >= SOSO_TABLEAU1 && m.to <= SOSO_TABLEAU7);
		soso_int_t suit = m.from - SOSO_FOUNDATION1C;
		++game->foundation_top[suit];
		--game->tableau_top[m.to - SOSO_TABLEAU1];
	}
	else {
		assert(0);
	}
	soso_clean_game(game);
	--ctx->moves_top;
}

void soso_internal_make_move(soso_ctx_t *ctx, soso_game_t *game, soso_move_t m) {
	if ((m.extra & SOSO_AUTO_MOVE) > 0) ++ctx->automoves_count;

	if (m.from == SOSO_STOCK_WASTE) {
		bool move_left = false;
		if (m.to == SOSO_STOCK_WASTE) {
			// Draw
			for (int i = 0; i < m.count; ++i) {
				if (!soso_internal_draw(game, ctx->draw_count)) assert(0);
			}
		}
		else if (m.to >= SOSO_TABLEAU1 && m.to <= SOSO_TABLEAU7) {
			// Waste -> Tableau
			assert(game->stock_cur < game->stock_count);
			soso_int_t i = m.to - SOSO_TABLEAU1;
			game->tableau[i][game->tableau_top[i]++] = game->stock[game->stock_cur];
			move_left = true;
		}
		else if (m.to >= SOSO_FOUNDATION1C && m.to <= SOSO_FOUNDATION4H) {
			// Waste -> Foundation
			assert(game->stock_cur < game->stock_count);
			soso_int_t i = m.to - SOSO_FOUNDATION1C;
			++game->foundation_top[i];
			move_left = true;
		}
		else {
			assert(0);
		}
		if (move_left) {
			for (int i = game->stock_cur; i < game->stock_count - 1; ++i)
				game->stock[i] = game->stock[i + 1];
			--(game->stock_count);
		}
	}
	else if (m.from >= SOSO_TABLEAU1 && m.from <= SOSO_TABLEAU7) {
		// Tableau -> ?
		if (m.to == m.from && (m.extra & SOSO_FLIP) > 0) {
			// Flip
			soso_int_t to = m.to - SOSO_TABLEAU1;
			--(game->tableau_up[to]);
		}
		else if (m.to >= SOSO_TABLEAU1 && m.to <= SOSO_TABLEAU7) {
			// Tableau -> Tableau
			soso_int_t to = m.to - SOSO_TABLEAU1;
			soso_int_t from = m.from - SOSO_TABLEAU1;
			for (int i = 0; i < m.count; ++i) {
				game->tableau[to][game->tableau_top[to]++] =
				    game->tableau[from][(game->tableau_top[from] - m.count) + i];
				game->tableau[from][(game->tableau_top[from] - m.count) + i] = SOSO_EMPTY_CARD;
			}
			game->tableau_top[from] -= m.count;
		}
		else if (m.to >= SOSO_FOUNDATION1C && m.to <= SOSO_FOUNDATION4H) {
			// Tableau -> Foundation
			++game->foundation_top[m.to - SOSO_FOUNDATION1C];
			--game->tableau_top[m.from - SOSO_TABLEAU1];
		}
		else {
			assert(0);
		}
	}
	else if (m.from >= SOSO_FOUNDATION1C && m.from <= SOSO_FOUNDATION4H) {
		assert(m.to >= SOSO_TABLEAU1 && m.to <= SOSO_TABLEAU7);
		soso_int_t suit = m.from - SOSO_FOUNDATION1C;
		--game->foundation_top[suit];
		game->tableau[m.to - SOSO_TABLEAU1][game->tableau_top[m.to - SOSO_TABLEAU1]++] =
		    soso_internal_make_card(suit, game->foundation_top[suit]);
	}
	else {
		assert(0);
	}
	soso_clean_game(game);
}

bool soso_solve(soso_ctx_t *ctx, soso_game_t *game) {
	ctx->visited =
	    sht_init_alloc(sizeof(uint32_t), ctx->max_visited, SOSO_HASH_SEED, ctx->alloc, ctx->free);
	int states_visited = 0;
	while (states_visited < ctx->max_visited) {
		soso_make_auto_moves(ctx, game);
		soso_internal_update_available_moves(ctx, game, false);
		bool move_made = false;
		soso_clean_game(game); // to get consistent state hash
		for (int i = 0; i < ctx->moves_available_top; ++i) {
			uint32_t h = soso_internal_state_hash(game, &ctx->moves_available[i]);
			if (sht_get(ctx->visited, &h, sizeof(uint32_t)) != NULL) continue;
			int last = ctx->moves_top - 1;
			int check = 3;
			bool skip = false;
			for (;;) {
				if (last < 0 || check == 0) break;
				if ((ctx->moves[last].extra & SOSO_AUTO_MOVE) > 0) {
					--last;
					continue;
				}
				soso_move_t lm = ctx->moves[last];
				soso_move_t nm = ctx->moves_available[i];
				if (lm.to == nm.from && lm.from == nm.to && lm.count == nm.count) {
					skip = true;
					break;
				}
				--check;
			}
			if (skip) continue;
			move_made = true;
			soso_move_t m = ctx->moves_available[i];
			int from = m.from;
			int to = m.to;
			int count = m.count;
			int extra = m.extra;
			soso_internal_add_move(ctx, ctx->moves_available[i], false);
			soso_internal_make_move(ctx, game, ctx->moves_available[i]);
			sht_set(ctx->visited, &h, sizeof(uint32_t), &h);
			++states_visited;
			break;
		}
		if (!move_made) {
			if (ctx->moves_top - ctx->automoves_count == 0) break;
			soso_internal_revert_to_last_move(ctx, game);
			continue;
		}
		if (soso_internal_game_solved(game)) return true;
	}
	return false;
}

// Internal Helpers

void soso_clean_game(soso_game_t *game) {
	for (int i = game->stock_count; i < 24; ++i) game->stock[i] = SOSO_EMPTY_CARD;
	for (int i = 0; i < 7; ++i)
		for (int j = game->tableau_top[i]; j < 19; ++j) game->tableau[i][j] = SOSO_EMPTY_CARD;
}

uint32_t soso_internal_state_hash(const soso_game_t *game, const soso_move_t *move) {
	uint32_t game_hash;
	MurmurHash3_x86_32(game, sizeof(soso_game_t), 42, &game_hash);
	return game_hash ^ *(const uint32_t *)move;
}

void soso_internal_revert_to_last_move(soso_ctx_t *ctx, soso_game_t *game) {
	for (;;) {
		if (ctx->moves_top == 0) break;
		soso_internal_undo_move(ctx, game);
		if ((ctx->moves[ctx->moves_top].extra & SOSO_AUTO_MOVE) == 0) break;
	}
}

bool soso_internal_game_solved(const soso_game_t *game) {
	soso_int_t total = 0;
	for (int i = 0; i < 4; ++i) total += game->foundation_top[i];
	return total == 52;
}

soso_int_t soso_internal_make_card(soso_int_t suit, soso_int_t value) {
	return (suit << 5) | value;
}

soso_int_t soso_internal_get_waste_card(const soso_game_t *game) {
	if (game->stock_count > 0 && game->stock_count > game->stock_cur)
		return game->stock[game->stock_cur];
	return -1;
}

soso_int_t soso_internal_cvalue(soso_int_t card) {
	return card & 0x0f;
}

soso_int_t soso_internal_csuit(soso_int_t card) {
	soso_int_t s = (card & 0x7f) >> 5;
	assert(s < 4 && s >= 0);
	return s;
}

soso_int_t soso_internal_ccolor(soso_int_t card) {
	return soso_internal_csuit(card) & 1;
}

bool soso_internal_valid_card(soso_int_t card) {
	soso_uint_t value = card & 0x0f;
	return value < 13;
}

bool soso_internal_foundation_valid(soso_int_t card, const soso_int_t *foundation_top) {
	return soso_internal_valid_card(card) &&
	       foundation_top[soso_internal_csuit(card)] == soso_internal_cvalue(card);
}

bool soso_internal_tableau_valid(soso_int_t from, soso_int_t to) {
	return soso_internal_valid_card(from) && soso_internal_valid_card(to) &&
	       (soso_internal_cvalue(to) - soso_internal_cvalue(from) == 1) &&
	       soso_internal_ccolor(from) != soso_internal_ccolor(to);
}

int soso_internal_max_draws(const soso_ctx_t *ctx, const soso_game_t *game) {
	int maxdraw = game->stock_count + 1;
	if (ctx->draw_count > 1) {
		if ((game->stock_count - game->stock_cur) % ctx->draw_count == 0)
			maxdraw = ((game->stock_count - game->stock_cur) / ctx->draw_count +
			           game->stock_cur / ctx->draw_count) +
			          1;
		else
			maxdraw = (game->stock_cur / ctx->draw_count + game->stock_count / ctx->draw_count) + 1;
	}
	return maxdraw;
}

uint64_t soso_internal_random_get_in(uint64_t min, uint64_t max) {
	return soso_random_get() % (max + 1 - min) + min;
}
