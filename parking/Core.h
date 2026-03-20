#pragma once

class CorePt2
{
public:
	CorePt2() = default;
	CorePt2(double x, double y);

	double DistanceTo(const CorePt2 &p) const;

	static const CorePt2 ZeroPoint2;

	double x = 0.0;
	double y = 0.0;
};

CorePt2 operator-(const CorePt2 &lhs, const CorePt2 &rhs);

class CoreVector2
{
public:
	CoreVector2() = default;
	CoreVector2(double x, double y);
	explicit CoreVector2(const CorePt2 &pt);

	CoreVector2 operator-() const;

	double Length() const;
	void Normalise();
	double DotProduct(const CoreVector2 &v) const;
	double AngleBetween(const CoreVector2 &v) const;
	bool CompareTo(const CoreVector2 &v, double precision) const;
	double AngleBetween360(const CoreVector2 &v) const;
	void SetLength(double length);
	void RotateBy(double angle);

	static const CoreVector2 ZeroVector;
	static const CoreVector2 XVector;
	static const CoreVector2 YVector;

	double x = 0.0;
	double y = 0.0;
};

bool operator==(const CoreVector2 &lhs, const CoreVector2 &rhs);
CorePt2 operator+(const CorePt2 &lhs, const CoreVector2 &v);
CorePt2 operator-(const CorePt2 &lhs, const CoreVector2 &v);

class C2DMatrix
{
public:
	C2DMatrix() = default;

	void LoadIdentity();
	void MakeTranslate(double dx, double dy);
	void MakeRotate(double angle);
	void MultMatrix(C2DMatrix *m2);
	void CompositeRotate(double angle);
	void MakeRotateAboutPoint(const CorePt2 &o, double angle);
	void CompositeTranslate(double dx, double dy);
	void CompositeTranslate(const CoreVector2 &v);

	CorePt2 operator*(const CorePt2 &p) const;

	double m[3][3] = { {0, 0, 0}, {0, 0, 0}, {0, 0, 0} };
};

bool IsCounterClockwise(const std::vector<CorePt2> &ring);