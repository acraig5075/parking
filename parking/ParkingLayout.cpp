#include "pch.h"
#include "ParkingLayout.h"
#include "Struct.h"

/// ParkingParams class

ParkingParams::ParkingParams(double length, double width, double angle)
	: length(length)
	, width(width)
	, angle(angle)
{
}

/// ParkingBay class

ParkingBay::ParkingBay(const CorePt3 &a, const CorePt3 &b, const CorePt3 &c, const CorePt3 &d)
{
	m_corners[0] = a;
	m_corners[1] = b;
	m_corners[2] = c;
	m_corners[3] = d;
}

PaintLine::PaintLine(const CorePt3 &start, const CorePt3 &end)
	: start(start)
	, end(end)
{
}

/// ParkingLayout class

void CParkingLayout::Make(const std::vector<CorePt3> &path, const ParkingParams &params)
{
	size_t nRows = path.size();

	// Islands at ends
	std::vector<Cap> endCaps;
	endCaps = MakeEndCapsOCS(params);
	endCaps[0] = TransformCapToUCS(path[0], path[1], endCaps[0]);
	endCaps[1] = TransformCapToUCS(path[nRows - 1], path[nRows- 2], endCaps[1]);
	m_capsUCS.insert(m_capsUCS.end(), endCaps.cbegin(), endCaps.cend());

	// Islands at bends
	std::vector<Cap> bendCaps;
	bendCaps = MakeBendCaps(path, params);
	m_capsUCS.insert(m_capsUCS.end() - 1, bendCaps.cbegin(), bendCaps.cend());

	for (size_t i = 1; i < path.size(); ++i)
		{
		double span = path[i - 1].PlanDistanceTo(path[i]);
		double start = m_capsUCS[i - 1].m_succeedingBay;
		double stop = m_capsUCS[i].m_preceedingBay;

		// Parking bays
		std::vector<ParkingBay> bays;
		bays = MakeBaysOCS(span, start, stop, params);
		bays = TransformBaysToUCS(path[i - 1], path[i], bays);

		// New row
		ParkingRow row;
		row.m_start = path[i - 1];
		row.m_end = path[i];
		row.m_baysUCS = bays;
		row.m_arrow = MakeArrow(row.m_start, row.m_end, params);

		m_rowsUCS.push_back(row);
		}
}

static double GetBayDepth(const ParkingParams &params)
{
	const double theta = deg_rad(params.angle);

	if (PARMZERO(theta) || PARMEQ(RADIAN90, theta))
		return params.length;

	double ycomponent =  params.width / tan(theta);

	return sin(theta) * (params.length + ycomponent);
}

/// Parking bay layout calculation in an easy axis-aligned, origin-centred, Object Coordinate System
std::vector<ParkingBay> CParkingLayout::MakeBaysOCS(double totalSpan, double startOff, double stopOff, const ParkingParams &params) const
{
	std::vector<ParkingBay> baysOCS;

	// 90° is normal perpendicular parking bays, angled bays of 55° and 45° are common. Less than that is abnormal.
	double radians = deg_rad(params.angle);
	if (PARMLEZERO(radians) || PARMGT(radians, RADIAN90))
		radians = RADIAN90;

	const double sina = sin(radians);
	const double cosa = cos(radians);
	const double tana = tan(radians);
	const double singleSpan = GPARMZERO(radians) ? params.width : params.width / sina;

	double lcomp = params.width / tana; // extension along length

	double xoff = cosa * (params.length + lcomp);
	double yoff = sina * (params.length + lcomp);

	// Consider angled bays
	if (PARMNE(radians, RADIAN90))
		startOff += xoff;

	// Position the bays such that the remaining gap is averaged on both ends
	double remaining = std::fmod(totalSpan - startOff - stopOff, singleSpan);
	startOff += (remaining / 2);

	if (PARMGTZERO(singleSpan))
		{
		CorePt3 fl; // front left corner
		CorePt3 fr; // front right corner
		CorePt3 rr; // rear right corner
		CorePt3 rl; // rear left corner

		for (int i = 0;; ++i)
			{
			// ParkingParams::SINGLE_ROW
			rr.x = startOff + (i + 1) * singleSpan;
			rr.y = 0.0;
			rl.x = startOff + i * singleSpan;
			rl.y = rr.y;
			fr.x = rr.x - xoff;
			fr.y = rr.y + (ParkingParams::LHS == params.driveDirection ? -yoff : yoff);
			fl.x = rl.x - xoff;
			fl.y = rl.y + (ParkingParams::LHS == params.driveDirection ? -yoff : yoff);

			// Loop ending condition
			if (PARMGT(rr.x, totalSpan - stopOff))
				break;

			baysOCS.emplace_back(fl, fr, rr, rl);

			if (ParkingParams::DOUBLE_ROW == params.rows)
				{
				fr.x = rr.x + xoff;
				fr.y = rr.y - (ParkingParams::LHS == params.driveDirection ? -yoff : yoff);
				fl.x = rl.x + xoff;
				fl.y = rl.y - (ParkingParams::LHS == params.driveDirection ? -yoff : yoff);

				baysOCS.emplace_back(fl, fr, rr, rl);
				}
			}
		}

	for (auto &bay : baysOCS)
		{
		const auto &fl = bay.m_corners[0];
		const auto &fr = bay.m_corners[1];
		const auto &rr = bay.m_corners[2];
		const auto &rl = bay.m_corners[3];

		if (ParkingParams::FULL == params.sideLines)
			{
			bay.m_lines.emplace_back(rl, fl);
			bay.m_lines.emplace_back(rr, fr);
			}
		else if (ParkingParams::SHORT == params.sideLines)
			{
			auto side = CoreVector3{ fl - rl };
			side.SetLength(params.shortPaintLength);
			bay.m_lines.emplace_back(fl, fl - side);
			bay.m_lines.emplace_back(fr, fr - side);
			bay.m_lines.emplace_back(rr, rr + side);
			bay.m_lines.emplace_back(rl, rl + side);
			}

		if (ParkingParams::FULL == params.frontLine)
			{
			bay.m_lines.emplace_back(fl, fr);
			}
		else if (ParkingParams::SHORT == params.frontLine)
			{
			auto side = CoreVector3{ fr - fl };
			side.SetLength(params.shortPaintLength);
			bay.m_lines.emplace_back(fl, fl + side);
			bay.m_lines.emplace_back(fr, fr - side);
			}

		if (ParkingParams::FULL == params.rearLine)
			{
			bay.m_lines.emplace_back(rl, rr);
			}
		else if (ParkingParams::SHORT == params.rearLine)
			{
			auto side = CoreVector3{ rr - rl };
			side.SetLength(params.shortPaintLength);
			bay.m_lines.emplace_back(rl, rl + side);
			bay.m_lines.emplace_back(rr, rr - side);
			}
		}

	return baysOCS;
};

/// Transformation of OCS parking bay layout to the User Coordinate System, involves rotation and translation
/// @note
/// After this function completes the UCS parking bays are in m_baysUCS.
std::vector<ParkingBay> CParkingLayout::TransformBaysToUCS(const CorePt3 &p1, const CorePt3 &p2, const std::vector<ParkingBay> &baysOCS) const
{
	C3DMatrix mat = MakeTranslateAndRotate(p1, p2);

	std::vector<ParkingBay> baysUCS;

	auto TransformFunc = [&mat, &baysUCS](const ParkingBay & bayOCS)
		{
		ParkingBay bayUCS;

		std::transform(bayOCS.m_corners.begin(), bayOCS.m_corners.end(), bayUCS.m_corners.begin(), [&mat](const CorePt3 & cnr)
			{
			return mat * cnr;
			});

		bayUCS.m_lines.resize(bayOCS.m_lines.size());
		std::transform(bayOCS.m_lines.begin(), bayOCS.m_lines.end(), bayUCS.m_lines.begin(), [&mat](const PaintLine & line)
			{
			return PaintLine{ mat * line.start, mat * line.end };
			});

		baysUCS.push_back(bayUCS);
		};

	std::for_each(baysOCS.begin(), baysOCS.end(), TransformFunc);

	return baysUCS;
}

/// Build the transformation matrix to transform OCS coordinates to UCS
C3DMatrix CParkingLayout::MakeTranslateAndRotate(const CorePt3 &p1, const CorePt3 &p2) const
{
	CoreVector3 rotation{ p2 - p1 };
	CoreVector3 translate1{ p1 - CorePt3::ZeroPoint3 };
	CoreVector3 translate2 = GetBaselineTranslation();
	double angle = CoreVector3::XVector.AngleBetween360(CoreVector3::PlanVector, rotation);

	C3DMatrix transform;
	transform.MakeRotateVectorPoint(p1, CoreVector3::PlanVector, angle); // rotation
	transform.CompositeTranslate(translate1); // translation from origin
	transform.CompositeTranslate(translate2); // translation from parking bay baseline (front/rear)
	return transform;
}

CoreVector3 CParkingLayout::GetBaselineTranslation() const
{
	return CoreVector3::ZeroVector;
}

CImageData CParkingLayout::GetImageData(const ParkingParams &params) const
{
	std::vector<PixelLine> imgLines;
	std::vector<PixelLine> imgPaths;
	std::vector<PixelPolygon> imgPolygons;
	std::vector<PixelPolygon> imgArrows;

	const int scaleFactor = 10; // scale metres to decimetres

	// Bays
	if (params.showBays)
		{
		for (const auto &row : m_rowsUCS)
			{
			for (const auto &bay : row.m_baysUCS)
				{
				for (const auto &paintLine : bay.m_lines)
					{
					PixelLine imgLine;
					imgLine.start.x = paintLine.start.x * scaleFactor;
					imgLine.start.y = paintLine.start.y * scaleFactor;
					imgLine.end.x = paintLine.end.x * scaleFactor;
					imgLine.end.y = paintLine.end.y * scaleFactor;
					imgLines.push_back(imgLine);
					}
				}
			}
		}

	// Islands
	if (params.showCaps)
		{
		for (const auto &cap : m_capsUCS)
			{
			PixelPolygon imgPolygon;
			for (const auto &pt : cap.m_polygon)
				{
				PixelPoint imgPoint;
				imgPoint.x = pt.x * scaleFactor;
				imgPoint.y = pt.y * scaleFactor;
				imgPolygon.points.push_back(imgPoint);
				}
			imgPolygons.push_back(imgPolygon);
			}
		}

	// Arrows
	if (params.showArrows)
		{
		for (const auto &row : m_rowsUCS)
			{
			PixelPolygon imgPolygon;
			for (const auto &pt : row.m_arrow.m_polygon)
				{
				PixelPoint imgPoint;
				imgPoint.x = pt.x * scaleFactor;
				imgPoint.y = pt.y * scaleFactor;
				imgPolygon.points.push_back(imgPoint);
				}
			imgPolygon.movement[0].x = row.m_arrow.m_movement[0].x * scaleFactor;
			imgPolygon.movement[0].y = row.m_arrow.m_movement[0].y * scaleFactor;
			imgPolygon.movement[1].x = row.m_arrow.m_movement[1].x * scaleFactor;
			imgPolygon.movement[1].y = row.m_arrow.m_movement[1].y * scaleFactor;
			imgArrows.push_back(imgPolygon);
			}
		}

	// Paths
	if (params.showPaths)
		{
		for (const auto &row : m_rowsUCS)
			{
			PixelLine imgLine;
			imgLine.start.x = row.m_start.x * scaleFactor;
			imgLine.start.y = row.m_start.y * scaleFactor;
			imgLine.end.x = row.m_end.x * scaleFactor;
			imgLine.end.y = row.m_end.y * scaleFactor;
			imgPaths.push_back(imgLine);
			}
		}

	double minx = std::numeric_limits<double>::max();
	double miny = std::numeric_limits<double>::max();
	double maxx = std::numeric_limits<double>::lowest();
	double maxy = std::numeric_limits<double>::lowest();

	auto MinMaxLine = [&minx, &miny, &maxx, &maxy](const std::vector<PixelLine> &lines)
		{
		for (const auto &line : lines)
			{
			minx = std::min(minx, std::min(line.start.x, line.end.x));
			miny = std::min(miny, std::min(line.start.y, line.end.y));
			maxx = std::max(maxx, std::max(line.start.x, line.end.x));
			maxy = std::max(maxy, std::max(line.start.y, line.end.y));
			}
		};

	auto MinMaxPolygon = [&minx, &miny, &maxx, &maxy](const std::vector<PixelPolygon> &polygons)
		{
		for (const auto &polygon : polygons)
			{
			for (const auto &pt : polygon.points)
				{
				minx = std::min(minx, pt.x);
				miny = std::min(miny, pt.y);
				maxx = std::max(maxx, pt.x);
				maxy = std::max(maxy, pt.y);
				}
			}
		};

	MinMaxLine(imgLines);
	MinMaxLine(imgPaths);

	MinMaxPolygon(imgPolygons);
	MinMaxPolygon(imgArrows);

	CImageData imageData;
	imageData.lines = std::move(imgLines);
	imageData.polygons = std::move(imgPolygons);
	imageData.arrows = std::move(imgArrows);
	imageData.paths = std::move(imgPaths);
	imageData.minimum = PixelPoint{ minx, miny };
	imageData.maximum = PixelPoint{ maxx, maxy };
	return imageData;
}

std::vector<Cap> CParkingLayout::MakeEndCapsOCS(const ParkingParams &params) const
{
	CorePt3 fl; // front left corner
	CorePt3 fr; // front right corner
	CorePt3 rr; // rear right corner
	CorePt3 rl; // rear left corner

	fr.x = rr.x = params.capWidth;

	if (ParkingParams::LHS == params.driveDirection)
		fl.y = fr.y = -params.length - params.capFrontExt;
	else
		fl.y = fr.y = params.length + params.capFrontExt;

	Cap startCap;
	startCap.m_polygon.push_back(rl);
	startCap.m_polygon.push_back(rr);
	startCap.m_polygon.push_back(fr);
	startCap.m_polygon.push_back(fl);
	startCap.m_succeedingBay = params.capWidth;

	fr.x = rr.x = params.capWidth;

	if (ParkingParams::LHS == params.driveDirection)
		fl.y = fr.y = params.length + params.capFrontExt;
	else
		fl.y = fr.y = -params.length - params.capFrontExt;

	Cap stopCap;
	stopCap.m_polygon.push_back(rl);
	stopCap.m_polygon.push_back(rr);
	stopCap.m_polygon.push_back(fr);
	stopCap.m_polygon.push_back(fl);
	stopCap.m_preceedingBay = params.capWidth;

	return std::vector<Cap>{ startCap, stopCap };
}

Cap CParkingLayout::TransformCapToUCS(const CorePt3 &p1, const CorePt3 &p2, const Cap &capOCS) const
	{
	C3DMatrix mat = MakeTranslateAndRotate(p1, p2);

	Cap capUCS;

	for (auto &ptOCS: capOCS.m_polygon)
		{
		capUCS.m_polygon.emplace_back(mat * ptOCS);
		}

	capUCS.m_preceedingBay = capOCS.m_preceedingBay;
	capUCS.m_succeedingBay = capOCS.m_succeedingBay;

	return capUCS;
	}

static double GetAngleAtB(const CorePt3 &A, const CorePt3 &B, const CorePt3 &C, bool LHS)
{
	// Create vectors BA and BC for angle calculation
	double BA_x = A.x - B.x;
	double BA_y = A.y - B.y;
	double BC_x = C.x - B.x;
	double BC_y = C.y - B.y;

	// Calculate the angle between vectors BA and BC
	double dot = BA_x * BC_x + BA_y * BC_y;
	double cross = BA_x * BC_y - BA_y * BC_x;

	// Calculate the angle using atan2
	double angle = std::atan2(cross, dot);

	// atan2 returns angle in range [-π, π]
	// Convert angle to [0, 2π] range
	if (angle < 0)
		{
		angle += 2 * PI;
		}

	if (LHS)
		{
		return 2 * PI - angle;
		}
	else
		{
		return angle;
		}
}

std::vector<Cap> CParkingLayout::MakeBendCaps(const std::vector<CorePt3> &path, const ParkingParams &params) const
{
	std::vector<Cap> caps;

	for (size_t i = 2; i < path.size(); ++i)
		{
		CorePt3 A = path[i - 2];
		CorePt3 B = path[i - 1];
		CorePt3 C = path[i];

		bool baysOnBendLeft = params.RHS == params.driveDirection;
		double bendAngle = GetAngleAtB(A, B, C, baysOnBendLeft);
		double bisectAngle = 0.5 * bendAngle;

		double perpCapLength = params.length + params.capFrontExt;
		double bisectorLength = perpCapLength / sin(bisectAngle);
		double span = std::abs(perpCapLength / tan(bisectAngle));
		double capWidthRear = PARMLT(bendAngle, PI) ? span + params.capFrontExt : params.capFrontExt;

		CoreVector3 forw{ C - B };
		CoreVector3 back{ A - B };
		CoreVector3 forwPerp = forw;
		CoreVector3 backPerp = back;
		CoreVector3 bisector = forw;
		forwPerp.RotateBy(baysOnBendLeft ? RADIAN90 : -RADIAN90);
		backPerp.RotateBy(baysOnBendLeft ? -RADIAN90 : RADIAN90);
		bisector.RotateBy(baysOnBendLeft ? bisectAngle : -bisectAngle);

		forw.SetLength(capWidthRear);
		back.SetLength(capWidthRear);
		bisector.SetLength(bisectorLength);
		forwPerp.SetLength(perpCapLength);
		backPerp.SetLength(perpCapLength);

		Cap cap;
		cap.m_polygon.push_back(B);
		cap.m_polygon.push_back(B + forw);
		cap.m_polygon.push_back(B + forw + forwPerp);
		cap.m_polygon.push_back(B + bisector);
		cap.m_polygon.push_back(B + back + backPerp);
		cap.m_polygon.push_back(B + back);
		cap.m_preceedingBay = capWidthRear;
		cap.m_succeedingBay = capWidthRear;

		caps.push_back(cap);
		}

	return caps;
}

Arrow CParkingLayout::MakeArrow(const CorePt3 &start, const CorePt3 &stop, const ParkingParams &params) const
{
	CoreVector3 laneDir { stop - start };
	CoreVector3 perpDir = laneDir;
	laneDir.SetLength(0.5 * laneDir.Length());
	perpDir.RotateBy(ParkingParams::LHS == params.driveDirection ? -RADIAN90 : RADIAN90);
	perpDir.SetLength(GetBayDepth(params) + params.laneWidth * (params.oneWayLane ? 0.5 : 0.25));

	CorePt3 pt = start + laneDir + perpDir;
	CorePt3 scale{ 0.1, 0.25, 0 };

	// OCS arrow pointing downwards
	Arrow arrow;
	arrow.m_polygon.emplace_back(pt.x, pt.y, 0.0);
	arrow.m_polygon.emplace_back(pt.x + 8 * scale.x, pt.y + 8 * scale.y, 0.0);
	arrow.m_polygon.emplace_back(pt.x + 3 * scale.x, pt.y + 8 * scale.y, 0.0);
	arrow.m_polygon.emplace_back(pt.x + 3 * scale.x, pt.y + 16 * scale.y, 0.0);
	arrow.m_polygon.emplace_back(pt.x - 3 * scale.x, pt.y + 16 * scale.y, 0.0);
	arrow.m_polygon.emplace_back(pt.x - 3 * scale.x, pt.y + 8 * scale.y, 0.0);
	arrow.m_polygon.emplace_back(pt.x - 8 * scale.x, pt.y + 8 * scale.y, 0.0);
	arrow.m_polygon.emplace_back(pt.x, pt.y, 0.0);

	double angle = CoreVector3::XVector.AngleBetween360(CoreVector3::PlanVector, laneDir);
	angle += RADIAN90;

	C3DMatrix mat;
	mat.MakeRotateVectorPoint(pt, CoreVector3::PlanVector, angle);

	// Transform arrow to UCS
	for (auto &point : arrow.m_polygon)
		{
		point = mat * point;
		}

	// Movement vector for animation
	arrow.m_movement[0].x = (arrow.m_polygon[3].x + arrow.m_polygon[4].x) / 2.0;
	arrow.m_movement[0].y = (arrow.m_polygon[3].y + arrow.m_polygon[4].y) / 2.0;
	arrow.m_movement[1] = arrow.m_polygon[0];

	return arrow;
}
