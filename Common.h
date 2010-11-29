/***************************************************************************
 *   This file is part of Robocut.                                         *
 *   Copyright (C) 2010 Tim Hutt <tdhutt@gmail.com>                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.              *
 ***************************************************************************/

#pragma once

/// Result Class

#include <string>
#include <sstream>
#include <cstdio>
#include <vector>
#include <cstdlib>

using std::vector;
using std::string;

typedef string::size_type stringpos;

/// Error Handling

class Error
{
public:
	explicit Error(bool S) : s(S) { }
	explicit Error(const char* C) : s(false), m(C) { }
	Error(const string& msg) : s(false), m(msg) { }
	operator bool() { return s; } // TODO: Use bool hack from boost so int i = Error(); doesn't work.
	string message() { if (m.empty()) return "Unspecified Error"; return m; }

private:
	bool s;
	string m;
};

const Error Success(true);
const Error Failure(false);

/** Integer to string. */
inline string ItoS(int I)
{
	std::stringstream S;
	S << I;
	return S.str();
}

/** Unsigned integer to string. */
inline string UItoS(unsigned int I)
{
	std::stringstream S;
	S << I;
	return S.str();
}

/** Unsigned long long to string. */
inline string ULLtoS(unsigned long long I)
{
	char out[128];
	sprintf(out, "%lld", I);
	return out;
}

/** String to integer, returns Fail on fail. */
inline int StoI(const string& S, int Fail = 0)
{
	char* EP;
	const char* P = S.c_str();
	int R = strtol(P, &EP, 0);
	if (EP == P)
		return Fail;
	return R;
}

/** String to unsigned integer, returns Fail on fail. */
inline unsigned int StoUI(const string& S, unsigned int Fail = 0)
{
	char* EP;
	const char* P = S.c_str();
	int R = strtoul(P, &EP, 0);
	if (EP == P)
		return Fail;
	return R;
}

/** String to unsigned integer, returns Fail on fail. */
inline unsigned int StoULL(const string& S, unsigned long long Fail = 0)
{
	char* EP;
	const char* P = S.c_str();
	int R = strtoull(P, &EP, 0);
	if (EP == P)
		return Fail;
	return R;
}

/** Get an environmental variable. */
inline string GetEnv(const string& Var)
{
	char* V = getenv(Var.c_str());
	if (!V)
		return "";
	return V;
}
