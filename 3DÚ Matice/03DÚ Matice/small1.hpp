#pragma once

#include <iosfwd>
#include <utility>
#include <vector>
#include <string>

using namespace std;

/*
 * Parses free formish matrix from stream.
 * The format is line oriented -> single line in stream is a single row in matrix.
 * Columns are separated by arbitrary non-newline whitespace.
 *
 * Returns the matrix dimensions as a pair, with first element being the number of rows and second the number of columns.
 *
 * Empty stream is a matrix of dimensions {0, 0}, matrix with inconsistent number of elements across rows should throw
 * invalid_argument.
 *
 * Worth 1 point.
 */
pair<int, int> parse_matrix(istream& in);

/*
 * Tabulates strings from the input vector into the output stream.
 * The table has two columns and minimal required number of rows.
 * The columns have equal width and the width is equal to the longest string + 1.
 * The strings are right justified.
 *
 * Example:
 * vector{"a", "b", "c"}
 * -> 
 * -------
 * | a| b|
 * | c|  |
 * -------
 *
 * Worth 1 point.
 */
void print_table(ostream& out, const vector<string>& vec);

/*
 * Validates a string representing a single line.
 * The expected format is [true/false] [octal number] [decimal number] [hexadecimal number]
 *
 * Returns true if the first position contains true and the three numbers have equal value
 *              or if the first position contains false and the three numbers do not have equal value;
 *              else returns false
 *
 * Worth 1 point.
 */
bool validate_line(const string& str);

/*
 * Returns string representing the largest of comma separated numbers inside a stream.
 * 
 * All input numbers will be positive and will not contain leading zeroes.
 *
 * Worth 1 point.
 */
string max_number(istream& in);
