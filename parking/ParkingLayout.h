#pragma once
#include "Core.h"
#include <optional>

struct ParkingParams
{
	enum DriveDirection { LHS, RHS };
	enum LayoutRows { SINGLE_ROW, DOUBLE_ROW };
	enum PaintLines { NONE, FULL, SHORT };

	ParkingParams() = default;
	ParkingParams(double length, double width, double angle);

	DriveDirection driveDirection = LHS;
	LayoutRows rows = SINGLE_ROW;

	PaintLines sideLines = SHORT;
	PaintLines frontLine = SHORT;
	PaintLines rearLine = SHORT;
	double shortPaintLength = 0.5;

	double length = 5.0;
	double width = 2.5;
	double angle = 90.0;

	double capWidth = 2.0;
	double capFrontExt = 0.5;

	double laneWidth = 10.0;
	bool oneWayLane = true;
	bool animateArrows = false;

	bool showBays = true;
	bool showCaps = true;
	bool showArrows = true;
	bool showPaths = false;
	};

struct PaintLine
{
	PaintLine() = default;
	PaintLine(const CorePt3 &start, const CorePt3 &end);

	CorePt3 start;
	CorePt3 end;
};

class Cap
{
public:
	std::vector<CorePt3> m_polygon;
	double m_preceedingBay = 0.0;
	double m_succeedingBay = 0.0;
};

class Arrow
{
public:
	std::vector<CorePt3> m_polygon;
	std::array<CorePt3, 2> m_movement;
};

class ParkingBay
{
public:
	enum CORNERS { SIDE1_FRONT = 0, SIDE2_FRONT, SIDE2_REAR, SIDE1_REAR };

	ParkingBay() = default;
	ParkingBay(const CorePt3 &a, const CorePt3 &b, const CorePt3 &c, const CorePt3 &d);

	std::array<CorePt3, 4> m_corners;
	std::vector<PaintLine> m_lines;
};

class ParkingRow
{
public:
	CorePt3 m_start;
	CorePt3 m_end;
	std::vector<ParkingBay> m_baysUCS;
	std::optional<size_t> m_startCap;
	std::optional<size_t> m_endCap;
	Arrow m_arrow;
};

struct PixelPoint
{
	double x = 0;
	double y = 0;
};

struct PixelLine
{
	PixelPoint start;
	PixelPoint end;
};

struct PixelPolygon
{
	std::vector<PixelPoint> points;
	std::array<PixelPoint, 2> movement;
};

struct CImageData
{
	std::vector<PixelLine> lines;
	std::vector<PixelPolygon> polygons;
	std::vector<PixelPolygon> arrows;
	PixelPoint minimum;
	PixelPoint maximum;
};

class CParkingLayout
{
public:
	CParkingLayout(void) = default;

	void Make(const std::vector<CorePt3> &path, const ParkingParams &params);
	CImageData GetImageData(const ParkingParams &params) const;

private:
	std::vector<ParkingBay> MakeBaysOCS(double span, double startOff, double stopOff, const ParkingParams &params) const;
	std::vector<ParkingBay> TransformBaysToUCS(const CorePt3 &p1, const CorePt3 &p2, const std::vector<ParkingBay> &baysOCS) const;

	std::vector<Cap> MakeEndCapsOCS(const ParkingParams &params) const;
	Cap TransformCapToUCS(const CorePt3 &p1, const CorePt3 &p2, const Cap &capOCS) const;

	std::vector<Cap> MakeBendCaps(const std::vector<CorePt3> &path, const ParkingParams &params) const;
	Arrow MakeArrow(const CorePt3 &start, const CorePt3 &stop, const ParkingParams &params) const;

	C3DMatrix MakeTranslateAndRotate(const CorePt3 &p1, const CorePt3 &p2) const;
	CoreVector3 GetBaselineTranslation() const;

private:
	std::vector<ParkingRow> m_rowsUCS;
	std::vector<Cap> m_capsUCS;
};
