#ifndef PJC_TESTS_DATA_SMALL6_HPP
#define PJC_TESTS_DATA_SMALL6_HPP

#include <vector>
#include <utility>
#include <string>

using test = std::pair<std::string, double>;
using test_data = std::vector<test>;

extern test_data simple_add;
extern test_data simple_sub;
extern test_data simple_div;
extern test_data simple_mul;
extern test_data simple_pow;
extern test_data complex_tests;
extern std::vector<test_data> simple_tests;

#endif