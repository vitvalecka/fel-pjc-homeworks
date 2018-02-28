#pragma once

#include <vector>
#include "expr.hpp"

using namespace std;

class Postfix : public expr
{
private:
	string exp;
	void write(ostream& out) const;
	vector<string> toPostfix() const;

	inline bool isNumber(char character) const;
	inline bool isOperator(char character) const;
	inline bool isLeftBracket(char character) const;
	inline bool isRightBracket(char character) const;
	inline bool hasHigherPrecedence(char chr, char stackChr) const;
	inline bool hasLowerPrecedence(char chr, char stackChr) const;
	inline bool hasSamePrecedence(char chr, char stackChr) const;
	inline bool isLeftAssociative(char character) const;

	inline bool isNumber(string expression) const;
	inline bool isOperator(string expression) const;

	inline string toStringWithPrerecision(double value) const;

public:
	Postfix(string expression);
	virtual ~Postfix() override = default;
	double evaluate() const override;
};
