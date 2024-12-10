#include "tests.h"

/* Test cases */
void test_cpp_table_basic();
void test_cpp_table_write();
void test_cpp_table_insert();
void test_cpp_table_erase();
void test_cpp_table_changing_cell();
void test_cpp_table_tbl_properties();
void test_cpp_table_cell_properties();
void test_cpp_table_text_styles();
void test_cpp_bug_fixes();

struct test_case bb_test_suite[] = {
    {"test_cpp_table_basic", test_cpp_table_basic},
    {"test_cpp_table_write", test_cpp_table_write},
    {"test_cpp_table_insert", test_cpp_table_insert},
    {"test_cpp_table_erase", test_cpp_table_erase},
    {"test_cpp_table_changing_cell", test_cpp_table_changing_cell},
    {"test_cpp_table_tbl_properties", test_cpp_table_tbl_properties},
    {"test_cpp_table_cell_properties", test_cpp_table_cell_properties},
    {"test_cpp_table_text_styles", test_cpp_table_text_styles},
    {"test_cpp_bug_fixes", test_cpp_bug_fixes},
};

auto run_bb_test_suite() -> int {
  int const bb_n_tests = sizeof(bb_test_suite) / sizeof(bb_test_suite[0]);
  run_test_suite("BLACK BOX TEST SUITE", bb_n_tests, bb_test_suite);
  return 0;
}

auto main() -> int {
  int status = 0;

  status |= run_bb_test_suite();

  return status;
}
