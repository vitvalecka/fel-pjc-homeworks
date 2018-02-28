#ifndef PJC_EXPR_HPP
#define PJC_EXPR_HPP 

#include <iosfwd>
#include <string>
#include <memory>

class expr
{
private:
    virtual void write(std::ostream& out) const = 0;
    friend std::ostream& operator<<(std::ostream&, const expr&);

public:
	virtual double evaluate() const = 0;
    virtual ~expr() = default;
};

/*
 * Creates expression tree from mathematical expression.
 *
 * Must respect the usual mathematical ordering of operators and parenthesis.
 * For details see https://cw.fel.cvut.cz/wiki/courses/a7b36pjc/ukoly/ukol_6
 */
std::unique_ptr<expr> create_expression_tree(const std::string& expression);

#endif