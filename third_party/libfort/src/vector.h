#ifndef VECTOR_H
#define VECTOR_H

#include <cstddef>
#include "fort_utils.h"

#define INVALID_VEC_INDEX ((size_t) - 1)

FT_INTERNAL
auto create_vector(size_t item_size, size_t capacity) -> f_vector_t *;

FT_INTERNAL
void destroy_vector(f_vector_t *);

FT_INTERNAL
auto vector_size(const f_vector_t *) -> size_t;

FT_INTERNAL
auto vector_capacity(const f_vector_t *) -> size_t;

FT_INTERNAL
auto vector_push(f_vector_t *, const void *item) -> int;

FT_INTERNAL
auto vector_insert(f_vector_t *, const void *item, size_t pos) -> int;

FT_INTERNAL
auto vector_split(f_vector_t *, size_t pos) -> f_vector_t *;

FT_INTERNAL
auto vector_at_c(const f_vector_t *vector, size_t index) -> const void *;

FT_INTERNAL
auto vector_at(f_vector_t *, size_t index) -> void *;

FT_INTERNAL
auto vector_swap(f_vector_t *cur_vec, f_vector_t *mv_vec, size_t pos) -> f_status;

FT_INTERNAL
void vector_clear(f_vector_t *);

FT_INTERNAL
auto vector_erase(f_vector_t *, size_t index) -> int;

#ifdef FT_TEST_BUILD
f_vector_t *copy_vector(f_vector_t *);
size_t vector_index_of(const f_vector_t *, const void *item);
#endif

#define VECTOR_AT(vector, pos, data_type) *(data_type *)vector_at((vector), (pos))

#define VECTOR_AT_C(vector, pos, const_data_type) *(const_data_type *)vector_at_c((vector), (pos))

#endif /* VECTOR_H */
