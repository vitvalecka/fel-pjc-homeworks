#include "small0.hpp"

int factorial(int n) {
	if (n > 1)
	{
		for (int i = n - 1; i > 1; i--)
		{
			n *= i;
		}
	}
	else if ((n == 1) || (n == 0))
	{
		n = 1;
	}
	
	return n;
}
