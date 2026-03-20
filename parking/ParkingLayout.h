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
	PaintLine(const CorePt2 &start, const CorePt2 &end);

	CorePt2 start;
	CorePt2 end;
};

class Cap
{
public:
	std::vector<CorePt2> m_polygon;
	double m_preceedingBay = 0.0;
	double m_succeedingBay = 0.0;
};

class Arrow
{
public:
	std::vector<CorePt2> m_polygon;
	std::array<CorePt2, 2> m_movement;
};

class ParkingBay
{
public:
	enum CORNERS { SIDE1_FRONT = 0, SIDE2_FRONT, SIDE2_REAR, SIDE1_REAR };

	ParkingBay() = default;
	ParkingBay(const CorePt2 &a, const CorePt2 &b, const CorePt2 &c, const CorePt2 &d);

	std::array<CorePt2, 4> m_corners;
	std::vector<PaintLine> m_lines;
};

class ParkingRow
{
public:
	CorePt2 m_start;
	CorePt2 m_end;
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
	std::vector<PixelLine> paths;
	std::vector<PixelPolygon> polygons;
	std::vector<PixelPolygon> arrows;
	PixelPoint minimum;
	PixelPoint maximum;
};

class CParkingLayout
{
public:
	CParkingLayout(void) = default;

	void Make(const std::string &wktGeometry, const ParkingParams &params);

	CImageData GetImageData(const ParkingParams &params) const;

private:
	void Make(const std::vector<CorePt2> &path, const ParkingParams &params);

	std::vector<ParkingBay> MakeBaysOCS(double span, double startOff, double stopOff, const ParkingParams &params) const;
	std::vector<ParkingBay> TransformBaysToUCS(const CorePt2 &p1, const CorePt2 &p2, const std::vector<ParkingBay> &baysOCS) const;

	std::vector<Cap> MakeEndCapsOCS(const ParkingParams &params) const;
	Cap TransformCapToUCS(const CorePt2 &p1, const CorePt2 &p2, const Cap &capOCS) const;

	std::vector<Cap> MakeBendCaps(const std::vector<CorePt2> &path, const ParkingParams &params) const;
	Arrow MakeArrow(const CorePt2 &start, const CorePt2 &stop, const ParkingParams &params) const;

	C2DMatrix MakeTranslateAndRotate(const CorePt2 &p1, const CorePt2 &p2) const;
	CoreVector2 GetBaselineTranslation() const;

private:
	std::vector<ParkingRow> m_rowsUCS;
	std::vector<Cap> m_capsUCS;
};
