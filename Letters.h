#pragma once

#include "Line.h"
#include "Common.h"

#include <string>
#include <iostream>

string LetterSpec(char c);

vector<Line> Letter(char c)
{
	string s = LetterSpec(c);
	
	vector<Line> ret;
	
	if (s.empty())
		return ret;
	
	string buff;
	double x;
	double y;
	Line l;
	for (int i = 0; i < s.length(); ++i)
	{
		if (isdigit(s[i]) || s[i] == '.')
		{
			buff += s[i];
		}
		else if (s[i] == ',')
		{
			if (!buff.empty())
				x = StoD(buff);
			buff.clear();
		}
		else if (s[i] == ';')
		{
			if (!buff.empty())
				y = StoD(buff);
			buff.clear();
			
			if (i > 1 && s[i-1] == ';')
			{
				ret.push_back(l);
				l.Clear();
			}
			else
			{
				l.Append(x, y);
			}
		}
	}
	if (!buff.empty())
		y = StoD(buff);
	l.Append(x, y);
	ret.push_back(l);

	return ret;
}

string LetterSpec(char c)
{
	switch (c)
	{
		case 'A':
			return "0,0;0.3,1;0.6,0;;0.15,0.5;0.45,0.5";
		case 'B':
			return "0,0;0,1;0.3,1;0.4,0.75;0.3,0.5;0,0.5;0.3,0.5;0.4,0.25;0.3,0;0,0";
		case 'C':
			return "0.5,1;0.2,0.8;0,0.5;0.2,0.2;0.5,0";
		case 'D':
			return "0,0;0,1;0.3,1;0.4,0.75;0.4,0.25;0.3,0;0,0";
		case 'E':
			return "0,0.6;0.3,0.6;;0.4,1;0,1;0,0;0.4,0";
		case 'F':
			return "0,0.6;0.3,0.6;;0.4,1;0,1;0,0";
		case 'G':
			return "0.2,1;0,0.8;0,0.2;0.3,0;0.4,0.2;0.4,0.5;0.3,0.5;0.45,0.5";
		case 'H':
			return "0,0;0,1;;0,0.6;0.4,0.6;;0.4,1;0.4,0";
		case 'I':
			return "0.2,1;0.2,0";
		case 'J':
			return "0.2,1;0.2,0.1;0,0";
		case 'K':
			return "0,1;0,0;;0.3,1;0,0.4;0.3,0";
		case 'L':
			return "0,1;0,0;0.3,0";
		case 'M':
			return "0,0;0,1;0.2,0.5;0.4,1;0.4,0";
		case 'N':
			return "0,0;0,1;0.3,0;0.3,1";
		case 'O':
			return "0,0.2;0,0.8;0.2,1;0.4,0.8;0.4,0.2;0.2,0;0,0.2";
		case 'P':
			return "0,0;0,1;0.3,1;0.4,0.9;0.4,0.8;0.3,0.7;0,0.6";
		case 'Q':
			return "0,0.2;0,0.8;0.2,1;0.4,0.8;0.4,0.2;0.2,0;0,0.2;;0.2,0.2;0.4,0";
		case 'R':
			return "0,0;0,1;0.3,1;0.4,0.9;0.4,0.8;0.3,0.7;0,0.6;0.5,0";
		case 'S':
			return "0,0.1;0.15,0;0.3,0.1;0.3,0.4;0,0.6;0.9;0.15,1;0.3,0.9";
		case 'T':
			return "0,1;0.4,1;;0.2,1;0.2,0";
		case 'U':
			return "0,1;0,0.2;0.1,0;0.2,0.1;0.2,1";
		case 'V':
			return "0,1;0.15,0;0.3,1";
		case 'W':
			return "0,1;0.05,0;0.2,0.5;0.35,0;0.4,1";
		case 'X':
			return "0,1;0.4,0;;0,0;0.4,1";
		case 'Y':
			return "0,1;0.2,0.5;;0,0;0.4,1";
		case 'Z':
			return "0,1;0.4,1;0,0;0.4,0";
		case 'a':
			return "";
		case 'b':
			return "";
		case 'c':
			return "";
		case 'd':
			return "";
		case 'e':
			return "";
		case 'f':
			return "";
		case 'g':
			return "";
		case 'h':
			return "";
		case 'i':
			return "";
		case 'j':
			return "";
		case 'k':
			return "";
		case 'l':
			return "";
		case 'm':
			return "";
		case 'n':
			return "";
		case 'o':
			return "";
		case 'p':
			return "";
		case 'q':
			return "";
		case 'r':
			return "";
		case 's':
			return "";
		case 't':
			return "";
		case 'u':
			return "";
		case 'v':
			return "";
		case 'w':
			return "";
		case 'x':
			return "";
		case 'y':
			return "";
		case 'z':
			return "";
		case '0':
			return "";
		case '1':
			return "";
		case '2':
			return "";
		case '3':
			return "";
		case '4':
			return "";
		case '5':
			return "";
		case '6':
			return "";
		case '7':
			return "";
		case '8':
			return "";
		case '9':
			return "";
		case '.':
			return "";
		case '!':
			return "";
		case ' ':
			return "";
		case '_':
			return "";
		case '=':
			return "";
		case '+':
			return "";
		case '*':
			return "";
		case '/':
			return "";
		case ':':
			return "";
		case ';':
			return "";
		case '\'':
			return "";
		case '"':
			return "";
		case '#':
			return "";
		case '~':
			return "";
		case '(':
			return "";
		case ')':
			return "";
		case ',':
			return "";
		case '?':
			return "";

	}
}
