#include "pch.h"
#include "Core.h"
#include "Struct.h"

#define UNIT_PRECIS 0.000001

const CorePt2 CorePt2::ZeroPoint2 = CorePt2{};
const CoreVector2 CoreVector2::ZeroVector = CoreVector2{};
const CoreVector2 CoreVector2::XVector = CoreVector2{ 1.0, 0.0 };
const CoreVector2 CoreVector2::YVector = CoreVector2{ 0.0, 1.0 };

CorePt2::CorePt2(double x, double y)
	: x(x)
	, y(y)
{
}

double CorePt2::DistanceTo(const CorePt2 &p) const
{
	return std::sqrt((p.x - x) * (p.x - x) + (p.y - y) * (p.y - y));
}

CorePt2 operator-(const CorePt2 &lhs, const CorePt2 &rhs)
{
	return CorePt2(lhs.x - rhs.x, lhs.y - rhs.y);
}

CoreVector2::CoreVector2(double x, double y)
	: x(x)
	, y(y)
{
}

CoreVector2::CoreVector2(const CorePt2 &pt)
	: x(pt.x)
	, y(pt.y)
{
}

CoreVector2 CoreVector2::operator-() const
{
	return CoreVector2(-x, -y);
}

double CoreVector2::Length() const
{
	return std::sqrt(x * x + y * y);
}

void CoreVector2::Normalise()
{
	double len = Length();
	if (PARMEQ(1.0, len))
		return;

	if (PARMGTZERO(len))
		{
		len = 1.0 / len;
		x *= len;
		y *= len;
		}
	else
		{
		x = 0.0;
		y = 0.0;
		}
}

double CoreVector2::DotProduct(const CoreVector2 &v) const
{
	return (x * v.x + y * v.y);
}

bool CoreVector2::CompareTo(const CoreVector2 &rhs, double tolerance) const
{
	return (abs(x - rhs.x) < tolerance &&
	        abs(y - rhs.y) < tolerance);
}

double CoreVector2::AngleBetween360(const CoreVector2 &v) const
{
	if (v == CoreVector2::ZeroVector)
		return 0.0;

	CoreVector2 a(x, y);
	a.Normalise();
	CoreVector2 b = v;
	b.Normalise();

	if (a.CompareTo(b, UNIT_PRECIS))
		return 0.0;
	if (a.CompareTo(-b, UNIT_PRECIS))
		return PI;

	double theta = AngleBetween(v);

	// 2D scalar cross product: negative means v is clockwise from this
	double cross = x * v.y - y * v.x;
	if (cross < 0.0)
		return RADIAN360 - theta;

	return theta;
}

double CoreVector2::AngleBetween(const CoreVector2 &v) const
{
	double a = DotProduct(v);
	double b = Length() * v.Length();
	if (PARMZERO(b))
		return 0.0;
	double ratio = a / b;
	if (ratio > 1)
		return 0.0;
	if (ratio < -1)
		return PI;

	return acos(ratio);
}

void CoreVector2::SetLength(double length)
{
	double thisLen = Length();
	if (PARMEQ(length, thisLen))
		return;

	if (!PARMZERO(thisLen))
		{
		double scalar = length / thisLen;
		x *= scalar;
		y *= scalar;
		}
	else
		{
		x = 0.0;
		y = 0.0;
		}
}

void CoreVector2::RotateBy(double angle)
{
	const double costheta = cos(angle);
	const double sintheta = sin(angle);

	double xa = costheta * x - sintheta * y;
	double ya = sintheta * x + costheta * y;

	x = xa;
	y = ya;
}

bool operator==(const CoreVector2 &lhs, const CoreVector2 &rhs)
{
	return (PARMEQ(lhs.x, rhs.x) && PARMEQ(lhs.y, rhs.y));
}

CorePt2 operator+(const CorePt2 &lhs, const CoreVector2 &v)
{
	return CorePt2(lhs.x + v.x, lhs.y + v.y);
}

CorePt2 operator-(const CorePt2 &lhs, const CoreVector2 &v)
{
	return CorePt2(lhs.x - v.x, lhs.y - v.y);
}

CorePt2 C2DMatrix::operator*(const CorePt2 &p) const
{
	double x = m[0][0] * p.x + m[0][1] * p.y + m[0][2];
	double y = m[1][0] * p.x + m[1][1] * p.y + m[1][2];
	return CorePt2(x, y);
}

void C2DMatrix::LoadIdentity()
{
	m[0][0] = 1.0; m[0][1] = 0.0; m[0][2] = 0.0;
	m[1][0] = 0.0; m[1][1] = 1.0; m[1][2] = 0.0;
	m[2][0] = 0.0; m[2][1] = 0.0; m[2][2] = 1.0;
}

void C2DMatrix::MakeTranslate(double dx, double dy)
{
	LoadIdentity();
	m[0][2] = dx;
	m[1][2] = dy;
}

void C2DMatrix::MakeRotate(double angle)
{
	LoadIdentity();
	const double ca = cos(angle);
	const double sa = sin(angle);
	m[0][0] =  ca; m[0][1] = -sa;
	m[1][0] =  sa; m[1][1] =  ca;
}

void C2DMatrix::MultMatrix(C2DMatrix *m2)
{
	double mr[3][3];
	for (int i = 0; i < 3; i++)
		{
		for (int j = 0; j < 3; j++)
			{
			mr[i][j] = m[i][0] * m2->m[0][j] +
			            m[i][1] * m2->m[1][j] +
			            m[i][2] * m2->m[2][j];
			}
		}

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			m[i][j] = mr[i][j];
}

void C2DMatrix::CompositeRotate(double angle)
{
	C2DMatrix tmp;
	tmp.MakeRotate(angle);
	MultMatrix(&tmp);
}

void C2DMatrix::MakeRotateAboutPoint(const CorePt2 &o, double angle)
{
	MakeTranslate(o.x, o.y);
	CompositeRotate(angle);
	CompositeTranslate(-o.x, -o.y);
}

void C2DMatrix::CompositeTranslate(double dx, double dy)
{
	C2DMatrix tmp;
	tmp.MakeTranslate(dx, dy);
	MultMatrix(&tmp);
}

void C2DMatrix::CompositeTranslate(const CoreVector2 &v)
{
	C2DMatrix tmp;
	tmp.MakeTranslate(v.x, v.y);
	MultMatrix(&tmp);
}

bool IsCounterClockwise(const std::vector<CorePt2> &ring)
{
	if (ring.size() < 3)
		return false;

	double signedArea = 0.0;

	for (size_t i = 0; i < ring.size(); ++i)
		{
		const CorePt2 &p1 = ring[i];
		const CorePt2 &p2 = ring[(i + 1) % ring.size()];

		signedArea += (p1.x * p2.y) - (p2.x * p1.y);
		}

	return signedArea > 0.0;
}