#ifndef CELL_H
#define CELL_H

#include <cstddef>
#include "fort_utils.h"

FT_INTERNAL
auto create_cell() -> f_cell_t *;

FT_INTERNAL
void destroy_cell(f_cell_t *cell);

FT_INTERNAL
auto copy_cell(f_cell_t *cell) -> f_cell_t *;

FT_INTERNAL
auto cell_vis_width(const f_cell_t *cell, const f_context_t *context) -> size_t;

FT_INTERNAL
auto cell_invis_codes_width(const f_cell_t *cell, const f_context_t *context) -> size_t;

FT_INTERNAL
auto hint_height_cell(const f_cell_t *cell, const f_context_t *context) -> size_t;

FT_INTERNAL
void set_cell_type(f_cell_t *cell, enum f_cell_type type);

FT_INTERNAL
auto get_cell_type(const f_cell_t *cell) -> enum f_cell_type;

FT_INTERNAL
auto cell_printf(f_cell_t *cell, size_t row, f_conv_context_t *cntx, size_t cod_width) -> int;

FT_INTERNAL
auto fill_cell_from_string(f_cell_t *cell, const char *str) -> f_status;

#ifdef FT_HAVE_WCHAR
FT_INTERNAL
auto fill_cell_from_wstring(f_cell_t *cell, const wchar_t *str) -> f_status;
#endif

FT_INTERNAL
auto fill_cell_from_buffer(f_cell_t *cell, const f_string_buffer_t *buf) -> f_status;

FT_INTERNAL
auto cell_get_string_buffer(f_cell_t *cell) -> f_string_buffer_t *;

#endif /* CELL_H */
