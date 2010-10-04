#include "Common.h"

const string& Alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
const string& Num = "0123456789";
const string& Punct = ".,!?:;#~@'[]{}\\/_-+=`\"£$%^&*()|";
const string& Space = " ";
const string& Underscore = "_";

string Securify(const string& S, const string& AllowedChars)
{
	string s = S;
	stringpos N;
	while ((N = s.find_first_not_of(AllowedChars)) != string::npos)
		s.erase(N, 1);

	return s;
}

// Escape \ to \\ and ' to \'.
string EscapeSingleQuotes(const string& S)
{
	string Ret;

	for (unsigned int i = 0; i < S.length(); ++i)
	{
		switch (S[i])
		{
		case '\\':
			Ret += "\\\\";
			break;
		case '\'':
			Ret += "\\'";
			break;
		default:
			Ret += S[i];
			break;
		}
	}

	return Ret;
}

// HTML escape.
string HTMLEscape(const string& S)
{
	string Ret;

	for (unsigned int i = 0; i < S.length(); ++i)
	{
		if (isalnum(S[i]) || S[i] == ' ' || S[i] == '.' || S[i] == '/' || S[i] == ':'
			|| S[i] == '_' || S[i] == '-' // TODO: Find safe/unsafe ones.
			|| static_cast<unsigned char>(S[i]) >= 128) // TODO: Check this (for utf8).
		{
			Ret += S[i];
		}
		else
		{
			Ret += "&#" + UItoS(static_cast<unsigned char>(S[i])) + ";";
		}
	}

	return Ret;
}

// Attribute escape.
string AttributeEscape(const string& S)
{
	string Ret;

	for (unsigned int i = 0; i < S.length(); ++i)
	{
		switch (S[i])
		{
		case '"':
			Ret += "&quot;";
			break;
		case '\'':
			Ret += "&apos;";
			break;
		case '&':
			Ret += "&amp;";
			break;
		case '<':
			Ret += "&lt;";
			break;
		case '>':
			Ret += "&gt;";
			break;
		default:
			Ret += S[i];
			break;
		}
	}

	return Ret;

}

