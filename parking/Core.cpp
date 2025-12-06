#include "pch.h"
#include "Core.h"
#include "Struct.h"

#define UNIT_PRECIS 0.000001

const CorePt3 CorePt3::ZeroPoint3 = CorePt3{};
const CoreVector3 CoreVector3::ZeroVector = CoreVector3{};
const CoreVector3 CoreVector3::PlanVector = CoreVector3{ 0.0, 0.0, 1.0 };
const CoreVector3 CoreVector3::XVector = CoreVector3{ 1.0, 0.0, 0.0 };
const CoreVector3 CoreVector3::YVector = CoreVector3{ 0.0, 1.0, 0.0 };

CorePt3::CorePt3(double x, double y, double z)
	: x(x)
	, y(y)
	, z(z)
{
}

double CorePt3::PlanDistanceTo(const CorePt3 &p) const
{
	return std::sqrt((p.x - x) * (p.x - x) + (p.y - y) * (p.y - y));
}

CorePt3 operator-(const CorePt3 &lhs, const CorePt3 &v2)
{
	return CorePt3(lhs.x - v2.x, lhs.y - v2.y, lhs.z - v2.z);
}

CoreVector3::CoreVector3(double x, double y, double z)
	: x(x)
	, y(y)
	, z(z)
{
}

CoreVector3::CoreVector3(const CorePt3 &pt)
	: x(pt.x)
	, y(pt.y)
	, z(pt.z)
{
}

CoreVector3 CoreVector3::operator-() const
{
	return CoreVector3(-x, -y, -z);
}

double CoreVector3::Length() const
{
	return std::sqrt(x * x + y * y + z * z);
}

void CoreVector3::Normalise()
{
	double len = Length();
	if (PARMEQ(1.0, len))
		return;

	if (PARMGTZERO(len))		// prevent div by zero
		{
		len = 1.0 / len;
		x *= len;
		y *= len;
		z *= len;
		}
	else
		{
		x = 0.0;
		y = 0.0;
		z = 0.0;
		}
}

double CoreVector3::DotProduct(const CoreVector3 &v) const
{
	return (x * v.x + y * v.y + z * v.z);
}

CoreVector3 CoreVector3::CrossProduct(const CoreVector3 &v) const
{
	return CoreVector3((y * v.z) - (z * v.y), (z * v.x) - (x * v.z), (x * v.y) - (y * v.x));
}

bool CoreVector3::CompareTo(const CoreVector3 &rhs, double tolerance) const
{
	return (abs(x - rhs.x) < tolerance &&
					abs(y - rhs.y) < tolerance &&
					abs(z - rhs.z) < tolerance);
}

double CoreVector3::AngleBetween360(CoreVector3 normal, const CoreVector3 &v) const
{
	if (v == CoreVector3::ZeroVector || normal == CoreVector3::ZeroVector)
		return 0.0;

	normal.Normalise();

	// check for same direction and opposite direction vectors
	// NB: Important because AngleBetween() will choke if they are very near parallel.
	CoreVector3 a(x, y, z);
	a.Normalise();
	CoreVector3 b = v;
	b.Normalise();

	// JH 12/6/2009 - Replaced == with CompareTo() (== uses PARMEQ() which is too finicky for unit vectors)
	//if (a == b)
	//	return 0.0;
	//else if (a == (b * -1.0))
	//	return PI;
	if (a.CompareTo(b, UNIT_PRECIS))					// 6 decimals is good for unit vectors
		return 0.0;
	if (a.CompareTo(-b, UNIT_PRECIS))
		return PI;

	double theta = AngleBetween(v);

	CoreVector3 check = CrossProduct(v);
	check.Normalise();
	//if (!normal.CompareTo(check))
	//	return RADIAN360 - theta;

	// JH 21/01/2008 - use new CompareTo() to avoid floating-point bug, which prevents old CompareTo() from working
	if (!normal.CompareTo(check, UNIT_PRECIS))					// 6 decimals is fine because we are dealing with unit vectors here
		return RADIAN360 - theta;

	return theta;
}

double CoreVector3::AngleBetween(const CoreVector3 &v) const
{
	double a = DotProduct(v);
	double b = Length() * v.Length();
	// [6415] - Check for division by zero
	if (PARMZERO(b))
		return 0.0;
	double ratio = a / b;
	// [6415] - Check ratio range: [-1..1]
	if (ratio > 1)
		return 0.0;
	if (ratio < -1)
		return PI;

	return acos(ratio);
}

void CoreVector3::SetLength(double length)
{
	double thisLen = Length();
	if (PARMEQ(length, thisLen))
		return;

	if (!PARMZERO(thisLen))
		{
		double scalar = length / thisLen;
		x *= scalar;
		y *= scalar;
		z *= scalar;
		}
	else
		{
		x = 0.0;
		y = 0.0;
		z = 0.0;
		}
}

// CCW rotation
void CoreVector3::RotateBy(double angle)
{
	const double costheta = cos(angle);
	const double sintheta = sin(angle);

	double xa = costheta * x - sintheta * y;
	double ya = sintheta * x + costheta * y;

	x = xa;
	y = ya;
}

bool operator==(const CoreVector3 &lhs, const CoreVector3 &v)
{
	return (PARMEQ(lhs.x, v.x) && PARMEQ(lhs.y, v.y) && PARMEQ(lhs.z, v.z));
}

CorePt3 operator+(const CorePt3 &lhs, const CoreVector3 &v)
{
	return CorePt3(lhs.x + v.x, lhs.y + v.y, lhs.z + v.z);
}

CorePt3 operator-(const CorePt3 &lhs, const CoreVector3 &v)
{
	return CorePt3(lhs.x - v.x, lhs.y - v.y, lhs.z - v.z);
}

CorePt3 C3DMatrix::operator*(const CorePt3 &p) const
{
	double x = m[0][0] * p.x + m[0][1] * p.y + m[0][2] * p.z + m[0][3];
	double y = m[1][0] * p.x + m[1][1] * p.y + m[1][2] * p.z + m[1][3];
	double z = m[2][0] * p.x + m[2][1] * p.y + m[2][2] * p.z + m[2][3];
	return CorePt3(x, y, z);
}

void C3DMatrix::LoadIdentity()
{
	m[0][0] = 1.0;
	m[0][1] = 0.0;
	m[0][2] = 0.0;
	m[0][3] = 0.0;
	m[1][0] = 0.0;
	m[1][1] = 1.0;
	m[1][2] = 0.0;
	m[1][3] = 0.0;
	m[2][0] = 0.0;
	m[2][1] = 0.0;
	m[2][2] = 1.0;
	m[2][3] = 0.0;
	m[3][0] = 0.0;
	m[3][1] = 0.0;
	m[3][2] = 0.0;
	m[3][3] = 1.0;
}

void C3DMatrix::MakeTranslate(double dx, double dy, double dz)
{
	// column 3 has x, y, z offsets
	LoadIdentity();
	m[0][3] = dx;
	m[1][3] = dy;
	m[2][3] = dz;
}

void C3DMatrix::MakeRotateArbitraryAxis(const CoreVector3 &axis, double angle)
{
	LoadIdentity();
	const double sa = sin(angle);
	const double ca = cos(angle);
	m[0][0] = ca + ((1 - ca) * axis.x * axis.x);
	m[0][1] = ((1 - ca) * axis.x * axis.y) - (sa * axis.z);
	m[0][2] = ((1 - ca) * axis.x * axis.z) + (sa * axis.y);

	m[1][0] = ((1 - ca) * axis.x * axis.y) + (sa * axis.z);
	m[1][1] = ca + ((1 - ca) * axis.y * axis.y);
	m[1][2] = ((1 - ca) * axis.y * axis.z) - (sa * axis.x);

	m[2][0] = ((1 - ca) * axis.x * axis.z) - (sa * axis.y);
	m[2][1] = ((1 - ca) * axis.y * axis.z) + (sa * axis.x);
	m[2][2] = ca + ((1 - ca) * axis.z * axis.z);
}

void C3DMatrix::MultMatrix(C3DMatrix *m2)
{
	double mr[4][4];
	for (int i = 0; i < 4; i++)			// each row //-V112
		{
		for (int j = 0; j < 4; j++)		// each column //-V112
			{
			mr[i][j] = m[i][0] * m2->m[0][j] +
								 m[i][1] * m2->m[1][j] +
								 m[i][2] * m2->m[2][j] +
								 m[i][3] * m2->m[3][j];
			}
		}

	// copy result back to this matrix
	for (int i = 0; i < 4; i++)			// each row //-V112
		{
		for (int j = 0; j < 4; j++)		// each column //-V112
			m[i][j] = mr[i][j];
		}
}


void C3DMatrix::CompositeRotateArbitraryAxis(const CoreVector3 &axis, double angle)
{
	C3DMatrix tmp;
	tmp.MakeRotateArbitraryAxis(axis, angle);
	MultMatrix(&tmp);
}

void C3DMatrix::MakeRotateVectorPoint(const CorePt3 &o, const CoreVector3 &v, double angle)
{
	MakeTranslate(o.x, o.y, o.z);
	CompositeRotateArbitraryAxis(v, angle);
	CompositeTranslate(-o.x, -o.y, -o.z);
}

void C3DMatrix::CompositeTranslate(double dx, double dy, double dz)
{
	C3DMatrix tmp;
	tmp.MakeTranslate(dx, dy, dz);
	MultMatrix(&tmp);
}

void C3DMatrix::CompositeTranslate(const CoreVector3 &v)
{
	C3DMatrix tmp;
	tmp.MakeTranslate(v.x, v.y, v.z);
	MultMatrix(&tmp);
}