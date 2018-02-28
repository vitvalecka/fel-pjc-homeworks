#include <algorithm>
#include <iostream>
#include <sstream>
#include "small1.hpp"

using namespace std;

inline bool isFromNumber(char character)
{
	return ((character >= '0') && (character <= '9')) || (character == '-');
}

inline int parseLine(string line)
{
	int numberOfElements = 0;
	char thisChar;
	bool lastCharWasNumber = false;

	for (long int i = 0; i < line.length(); i++)
	{
		thisChar = line.at(i);

		if (!lastCharWasNumber && isFromNumber(thisChar))
		{
			numberOfElements++;
			lastCharWasNumber = true;
		}
		
		if (lastCharWasNumber && !isFromNumber(thisChar))
		{
			lastCharWasNumber = false;
		}
	}

	return numberOfElements;
}

pair<int, int> parse_matrix(istream& in)
{
	int rows = 0;
	int columns = 0;
	bool firstRead = true;
	int newColumns;

	string line = "";
	while (getline(in, line))
	{
		if (firstRead)
		{
			columns = parseLine(line);
			firstRead = false;
		}
		else
		{
			newColumns = parseLine(line);
			if (columns != newColumns)
			{
				throw invalid_argument("exception is thrown (inconsistent row sizes)");
			}
		}
		rows++;
	}

	return { rows,columns };
}

int longestStringInVector(vector<string> vec)
{
	unsigned int longestString = 0;

	for (unsigned int i = 0; i < vec.size(); i++)
	{
		if (vec[i].length() > longestString)
		{
			longestString = vec[i].length();
		}
	}

	return longestString;
}

void print_table(ostream& out, const vector<string>& vec)
{
	unsigned int longestString = longestStringInVector(vec) + 1;
	unsigned int lineLength = ((longestString + 1) * 2) + 1;
	int numberOfLastElement = 0;
	
	for (unsigned int i = 0; i < lineLength; i++)
	{
		out << "-";
	}
	out << endl;

	for (unsigned int i = 1; i <= vec.size(); i++)
	{
		out << "|";

		for (unsigned int j = longestString - vec[i - 1].length(); j > 0; j--)
		{
			out << " ";
		}

		out << vec[i - 1];

		if (i % 2 == 0)
		{
			out << "|" << endl;
		}

		numberOfLastElement++;
	}

	if (numberOfLastElement % 2 == 1)
	{
		out << "|";

		for (unsigned int i = longestString; i > 0; i--)
		{
			out << " ";
		}

		out << "|" << endl;
	}

	for (unsigned int i = 0; i < lineLength; i++)
	{
		out << "-";
	}
	out << endl;
}

bool validate_line(const string& str)
{
	vector<string> elements;
	istringstream iss(str);

	for (string s; iss >> s;)
	{
		elements.push_back(s);
	}

	if (elements.size() != 4)
	{
		return false;
	}

	if (elements[3].at(elements[3].length() - 1) < '0' || elements[3].at(elements[3].length() - 1) > 'f')
	{
		return false;
	}

	vector<int> numbers;
	try
	{
		numbers.push_back(stoi(elements[1], nullptr, 8));
		numbers.push_back(stoi(elements[2], nullptr, 10));
		numbers.push_back(stoi(elements[3], nullptr, 16));
	}
	catch (exception &e)
	{
		return false;
	}

	if (!elements[0].compare("true") && numbers[0] == numbers[1] && numbers[1] == numbers[2])
	{
		return true;
	}
	else if (!elements[0].compare("false") && (numbers[0] != numbers[1] || numbers[1] != numbers[2]))
	{
		return true;
	}
	
	return false;
}

string max_number(istream& in)
{
	string str;
	string max = "";
	vector<string> elements;

	getline(in, str);
	
	string foo;
	istringstream ss(str);

	while (getline(ss, foo, ','))
	{
		elements.push_back(foo);
	}

	for (string number : elements)
	{
		if (number.length() > max.length())
		{
			max = number;
		}
		else if (number.length() < max.length())
		{
			continue;
		}
		else
		{
			for (int i = 0; i < number.length(); i++)
			{
				if (number.at(i) > max.at(i))
				{
					max = number;
					break;
				}
			}
		}
	}
	
	return max;
}
