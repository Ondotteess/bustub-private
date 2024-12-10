#ifndef ARGPARSE_TEST_UTILITY_HPP
#define ARGPARSE_TEST_UTILITY_HPP

namespace testutility {
// Get value at index from std::list
template <typename T>
T get_from_list(const std::list<T>& a_list, size_t a_index) {
  if (a_list.size() > a_index) {
    auto t_iterator = a_list.begin();
    std::advance(t_iterator, a_index);
    return *t_iterator;
  }
  return T();
}
}

#endif //ARGPARSE_TEST_UTILITY_HPP
