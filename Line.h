#pragma once

#include <string>
#include <vector>

using namespace std;

class Point
{
public:
	Point(double X, double Y) : x(X), y(Y) { }
	~Point() { }
	
	Point operator-(const Point& p) const { return Point(x - p.x, y - p.y); }
	Point operator+(const Point& p) const { return Point(x + p.x, y + p.y); }
	Point operator*(const double& d) const { return Point(x * d, y * d); }
	Point operator/(const double& d) const { return Point(x / d, y / d); }
	
	const Point& operator-=(const Point& p) { x -= p.x; y -= p.y; return *this; }
	const Point& operator+=(const Point& p) { x += p.x; y += p.y; return *this; }
	const Point& operator*=(const double& d) { x *= d; y *= d; return *this; }
	const Point& operator/=(const double& d) { x /= d; y /= d; return *this; }
	
	double x;
	double y;
};

class Line
{
public:
	Line() { }
	~Line() { }
	
	void Append(const Point& p) { points.push_back(p); }
	void Append(double x, double y) { Append(Point(x, y)); }
	void Clear() { points.clear(); }
	void AppendOffset(const Point& p)
	{
		if (points.empty())
			points.push_back(p);
		else
			points.push_back(p + points[points.size()-1]);
	}
	void AppendOffset(double x, double y) { AppendOffset(Point(x, y)); }
	int Size() const { return points.size(); }
	
	vector<Point> points;
};
