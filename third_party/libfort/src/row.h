#ifndef ROW_H
#define ROW_H

#include <cstdarg>
#include "fort.h"
#include "fort_utils.h"
#include "properties.h"
#ifdef FT_HAVE_WCHAR
#include <cwchar>
#endif

FT_INTERNAL
auto create_row() -> f_row_t *;

FT_INTERNAL
void destroy_row(f_row_t *row);

FT_INTERNAL
auto copy_row(f_row_t *row) -> f_row_t *;

FT_INTERNAL
auto split_row(f_row_t *row, size_t pos) -> f_row_t *;

// Delete range [left; right] of cells (both ends included)
FT_INTERNAL
auto ft_row_erase_range(f_row_t *row, size_t left, size_t right) -> int;

FT_INTERNAL
auto create_row_from_string(const char *str) -> f_row_t *;

FT_INTERNAL
auto create_row_from_fmt_string(const struct f_string_view *fmt, va_list *va_args) -> f_row_t *;

FT_INTERNAL
auto columns_in_row(const f_row_t *row) -> size_t;

FT_INTERNAL
auto get_cell(f_row_t *row, size_t col) -> f_cell_t *;

FT_INTERNAL
auto get_cell_c(const f_row_t *row, size_t col) -> const f_cell_t *;

FT_INTERNAL
auto get_cell_and_create_if_not_exists(f_row_t *row, size_t col) -> f_cell_t *;

FT_INTERNAL
auto create_cell_in_position(f_row_t *row, size_t col) -> f_cell_t *;

FT_INTERNAL
auto swap_row(f_row_t *cur_row, f_row_t *ins_row, size_t pos) -> f_status;

FT_INTERNAL
auto insert_row(f_row_t *cur_row, f_row_t *ins_row, size_t pos) -> f_status;

FT_INTERNAL
auto group_cell_number(const f_row_t *row, size_t master_cell_col) -> size_t;

FT_INTERNAL
auto get_row_cell_types(const f_row_t *row, enum f_cell_type *types, size_t types_sz) -> int;

FT_INTERNAL
auto row_set_cell_span(f_row_t *row, size_t cell_column, size_t hor_span) -> f_status;

FT_INTERNAL
auto print_row_separator(f_conv_context_t *cntx, const size_t *col_width_arr, size_t cols, const f_row_t *upper_row,
                         const f_row_t *lower_row, enum f_hor_separator_pos separatorPos,
                         const f_separator_t *sep) -> int;

FT_INTERNAL
auto snprintf_row(const f_row_t *row, f_conv_context_t *cntx, size_t *col_width_arr, size_t col_width_arr_sz,
                  size_t row_height) -> int;

#ifdef FT_HAVE_WCHAR
FT_INTERNAL
auto create_row_from_wstring(const wchar_t *str) -> f_row_t *;
#endif

#endif /* ROW_H */
