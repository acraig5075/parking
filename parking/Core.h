#pragma once

class CorePt3
{
public:
	CorePt3() = default;
	CorePt3(double x, double y, double z);

	double PlanDistanceTo(const CorePt3 &p) const;

	static const CorePt3 ZeroPoint3;

	double x = 0.0;
	double y = 0.0;
	double z = 0.0;
};

CorePt3 operator-(const CorePt3 &lhs, const CorePt3 &v2);

class CoreVector3
{
public:
	CoreVector3() = default;
	CoreVector3(double x, double y, double z);
	explicit CoreVector3(const CorePt3 &pt);

	CoreVector3 operator-() const;

	double Length() const;
	void Normalise();
	double DotProduct(const CoreVector3 &v) const;
	CoreVector3 CrossProduct(const CoreVector3 &v) const;
	double AngleBetween(const CoreVector3 &v) const;
	bool CompareTo(const CoreVector3 &v, double precision) const;
	double AngleBetween360(CoreVector3 normal, const CoreVector3 &v) const;
	void SetLength(double length);
	void RotateBy(double angle);

	static const CoreVector3 ZeroVector;
	static const CoreVector3 PlanVector;
	static const CoreVector3 XVector;
	static const CoreVector3 YVector;

	double x = 0.0;
	double y = 0.0;
	double z = 0.0;
};

bool operator==(const CoreVector3 &lhs, const CoreVector3 &v);
CorePt3 operator+(const CorePt3 &lhs, const CoreVector3 &v);
CorePt3 operator-(const CorePt3 &lhs, const CoreVector3 &v);

class C3DMatrix
{
public:
	C3DMatrix() = default;

	void LoadIdentity();
	void MakeTranslate(double dx, double dy, double dz);
	void MakeRotateArbitraryAxis(const CoreVector3 &axis, double angle);
	void MultMatrix(C3DMatrix *m2);
	void CompositeRotateArbitraryAxis(const CoreVector3 &axis, double angle);
	void MakeRotateVectorPoint(const CorePt3 &o, const CoreVector3 &v, double angle);
	void CompositeTranslate(double dx, double dy, double dz);
	void CompositeTranslate(const CoreVector3 &v);

	CorePt3 operator*(const CorePt3 &p) const;

	double m[4][4] = { {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} };
};

bool IsCounterClockwise(const std::vector<CorePt3> &ring);