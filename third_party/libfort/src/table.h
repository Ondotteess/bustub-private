#ifndef TABLE_H
#define TABLE_H

#include <cstddef>
#include "fort_utils.h"
#include "libfort/src/fort.h"

struct ft_table {
  f_vector_t *rows;
  f_table_properties_t *properties;
  f_string_buffer_t *conv_buffer;
  size_t cur_row;
  size_t cur_col;
  f_vector_t *separators;
};

FT_INTERNAL
auto create_separator(int enabled) -> f_separator_t *;

FT_INTERNAL
void destroy_separator(f_separator_t *sep);

FT_INTERNAL
auto copy_separator(f_separator_t *sep) -> f_separator_t *;

FT_INTERNAL
auto get_table_sizes(const ft_table_t *table, size_t *rows, size_t *cols) -> f_status;

FT_INTERNAL
auto get_row(ft_table_t *table, size_t row) -> f_row_t *;

FT_INTERNAL
auto get_row_c(const ft_table_t *table, size_t row) -> const f_row_t *;

FT_INTERNAL
auto get_row_and_create_if_not_exists(ft_table_t *table, size_t row) -> f_row_t *;

FT_INTERNAL
auto get_cur_str_buffer_and_create_if_not_exists(ft_table_t *table) -> f_string_buffer_t *;

FT_INTERNAL
auto table_rows_and_cols_geometry(const ft_table_t *table, size_t **col_width_arr_p, size_t *col_width_arr_sz,
                                  size_t **row_height_arr_p, size_t *row_height_arr_sz,
                                  enum f_geometry_type geom) -> f_status;

FT_INTERNAL
auto table_geometry(const ft_table_t *table, size_t *height, size_t *width) -> f_status;

/*
 * Returns geometry in codepoints(characters) (include codepoints of invisible
 * elements: e.g. styles tags).
 */
FT_INTERNAL
auto table_internal_codepoints_geometry(const ft_table_t *table, size_t *height, size_t *width) -> f_status;

#endif /* TABLE_H */
