#include <iostream>
#include <vector>
#include <cmath>
#include <sstream>
#include <iomanip>
#include "postfix.h"

using namespace std;


const bool DEBUG = false;


void Postfix::write(ostream& out) const {}

/*
Podmínky pro pøevod na postfix viz: http://csis.pace.edu/~wolf/CS122/infix-postfix.htm
*/
vector<string> Postfix::toPostfix() const
{
	vector<string> postfix;
	vector<char> ostack;
	bool isFirstNumber = true;
	double result = 0;
	int i = 0;

	while (i < exp.length())
	{
		char chr = exp.at(i);

		if (chr == ' ')
		{
			i++;
			continue;
		}
		else if (isNumber(chr))
		{
			string nstack = string(1, chr);
			i++;

			while ((i < exp.length()) && (isNumber(exp.at(i)) || (exp.at(i) == '.')))
			{
				nstack += exp.at(i);
				i++;
			}

			postfix.push_back(nstack);
			continue;
		}
		else if (isLeftBracket(chr))
		{
			ostack.push_back(chr);
		}
		else if (isRightBracket(chr))
		{
			char foo = ostack.back();
			ostack.pop_back();

			while (foo != '(')
			{
				postfix.push_back(string(1, foo));
				foo = ostack.back();
				ostack.pop_back();
			}

			i++;
			continue;
		}
		else if (isOperator(chr))
		{
			start:
			if ((ostack.empty()) || (ostack.back() == '('))
			{
				ostack.push_back(chr);
			}
			else if (hasHigherPrecedence(chr, ostack.back()))
			{
				ostack.push_back(chr);
			}
			else if (hasSamePrecedence(chr, ostack.back()))
			{
				if (!isLeftAssociative(chr))
				{
					postfix.push_back(string(1, ostack.back()));
					ostack.pop_back();
				}

				ostack.push_back(chr);
			}
			else if (hasLowerPrecedence(chr, ostack.back()))
			{
				postfix.push_back(string(1, ostack.back()));
				ostack.pop_back();
				goto start;
			}
		}

		i++;
	}

	while (!ostack.empty())
	{
		postfix.push_back(string(1, ostack.back()));
		ostack.pop_back();
	}

	if (DEBUG)
	{
		cout << exp << endl;

		for (string item : postfix)
		{
			cout << item << " ";
		}
		cout << endl;
	}

	return postfix;
}

inline bool Postfix::isNumber(char character) const
{
	return (((int)character >= (int)'0') && ((int)character <= (int)'9'));
}

inline bool Postfix::isOperator(char character) const
{
	return ((character == '+') || (character == '-') || (character == '*') || (character == '/') || (character == '^'));
}

inline bool Postfix::isLeftBracket(char character) const
{
	return character == '(';
}

inline bool Postfix::isRightBracket(char character) const
{
	return character == ')';
}

inline bool Postfix::hasHigherPrecedence(char chr, char stackChr) const
{
	if ((chr == '*' || chr == '/' || chr == '^') && (stackChr == '+' || stackChr == '-'))
	{
		return true;
	}
	else if ((chr == '^') && (stackChr == '*' || stackChr == '/'))
	{
		return true;
	}

	return false;
}

inline bool Postfix::hasLowerPrecedence(char chr, char stackChr) const
{
	if ((chr == '+' || chr == '-') && (stackChr == '*' || stackChr == '/' || stackChr == '^'))
	{
		return true;
	}
	else if ((chr == '*' || chr == '/') && (stackChr == '^'))
	{
		return true;
	}

	return false;
}

inline bool Postfix::hasSamePrecedence(char chr, char stackChr) const
{
	if ((chr == '+' || chr == '-') && (stackChr == '+' || stackChr == '-'))
	{
		return true;
	}
	else if ((chr == '*' || chr == '/') && (stackChr == '*' || stackChr == '/'))
	{
		return true;
	}
	else if (chr == '^' && stackChr == '^')
	{
		return true;
	}

	return false;
}

inline bool Postfix::isLeftAssociative(char character) const
{
	return character == '^';
}

inline bool Postfix::isNumber(string expression) const
{
	if (isNumber(expression.at(0)))
	{
		return true;
	}
	else if ((expression.length() > 1) && isNumber(expression.at(1)))
	{
		return true;
	}
	else
	{
		return false;
	}
}

inline bool Postfix::isOperator(string expression) const
{
	return isOperator(expression.at(0));
}


inline string Postfix::toStringWithPrerecision(double value) const
{
	stringstream s;
	s << fixed << setprecision(100) << value;
	return s.str();
}


Postfix::Postfix(string expression)
{
	this->exp = expression;
}


/*
Vyhodnocení viz: https://en.wikipedia.org/wiki/Reverse_Polish_notation#Example
*/
double Postfix::evaluate() const
{
	vector<string> eip = toPostfix();
	int i = 0;

	while (eip.size() > 1)
	{
		if (isNumber(eip[i]))
		{
			i++;
		}
		else if (isOperator(eip[i]))
		{
			double x = stod(eip[i - 2]);
			double y = stod(eip[i - 1]);
			char oper = eip[i].at(0);

			if (oper == '+')
			{
				eip[i - 1] = toStringWithPrerecision(x + y);
			}
			else if (oper == '-')
			{
				eip[i - 1] = toStringWithPrerecision(x - y);
			}
			else if (oper == '*')
			{
				eip[i - 1] = toStringWithPrerecision(x * y);
			}
			else if (oper == '/')
			{
				eip[i - 1] = toStringWithPrerecision(x / y);
			}
			else if (oper == '^')
			{
				eip[i - 1] = toStringWithPrerecision(pow(x, y));
			}

			eip.erase(eip.begin() + i);
			eip.erase(eip.begin() + i - 2);

			i = 0;
		}
	}

	if (DEBUG)
	{
		cout << eip[0] << endl;
	}

	return stod(eip[0]);
}