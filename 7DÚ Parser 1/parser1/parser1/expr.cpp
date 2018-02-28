#include "expr.hpp"
#include "postfix.h"

using namespace std;


ostream & operator<<(ostream& out, const expr& e)
{
    return out;
}


unique_ptr<expr> create_expression_tree(const string& expression)
{
	unique_ptr<expr> foo (new Postfix(expression));
    return foo;
}
