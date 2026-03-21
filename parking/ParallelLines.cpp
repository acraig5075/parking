#include "pch.h"
#include "ParallelLines.h"
#include "Struct.h"

LineSegment::LineSegment(const CorePt2 &start, const CorePt2 &end)
	: start(start)
	, end(end)
{
}

/// Compute the inward-offset (inset) polygon from a closed polygon.
/// For each edge the offset line is shifted inward by insetDist, then adjacent
/// offset lines are intersected to produce the new corner positions.
///
/// @param polygon   Closed polygon with start == end (duplicated last vertex).
/// @param insetDist Positive inset distance.
/// @return          Inset polygon vertices (n unique vertices, not duplicated).
static std::vector<CorePt2> ComputeInsetPolygon(
	const std::vector<CorePt2> &polygon,
	double insetDist)
{
	const size_t n = polygon.size() - 1; // unique vertex count
	if (n < 3)
		return {};

	bool ccw = IsCounterClockwise(polygon);

	// Inward normal direction:
	//   CCW polygon – interior is to the LEFT of each edge direction  → rotate +90°: (-dy,  dx)
	//   CW  polygon – interior is to the RIGHT of each edge direction → rotate -90°: ( dy, -dx)
	double sign = ccw ? 1.0 : -1.0;

	// Build one offset line (origin + direction) per edge
	struct OffsetLine
		{
		CorePt2 origin;
		CoreVector2 dir;
		};

	std::vector<OffsetLine> offsetLines(n);
	for (size_t i = 0; i < n; ++i)
		{
		const CorePt2 &p0 = polygon[i];
		const CorePt2 &p1 = polygon[(i + 1) % n];

		CoreVector2 e{ p1.x - p0.x, p1.y - p0.y };
		e.Normalise();

		// Inward normal: rotate e by sign*90°
		CoreVector2 normal{ -sign * e.y, sign * e.x };

		offsetLines[i].origin = CorePt2(p0.x + insetDist * normal.x,
																		p0.y + insetDist * normal.y);
		offsetLines[i].dir = e;
		}

	// Intersect adjacent offset lines to find inset corners
	std::vector<CorePt2> result(n);
	for (size_t i = 0; i < n; ++i)
		{
		const OffsetLine &prev = offsetLines[(i + n - 1) % n];
		const OffsetLine &curr = offsetLines[i];

		// Solve: prev.origin + t * prev.dir  ==  curr.origin + s * curr.dir
		double denom = prev.dir.x * curr.dir.y - prev.dir.y * curr.dir.x;
		if (PARMZERO(denom))
			{
			// Parallel adjacent edges – fall back to the offset origin of the current edge
			result[i] = curr.origin;
			}
		else
			{
			double dx = curr.origin.x - prev.origin.x;
			double dy = curr.origin.y - prev.origin.y;
			double t = (dx * curr.dir.y - dy * curr.dir.x) / denom;
			result[i] = CorePt2(prev.origin.x + t * prev.dir.x,
													prev.origin.y + t * prev.dir.y);
			}
		}

	return result;
}

/// Find all t-values where the infinite line (P + t*dir) crosses edges of the polygon.
/// Uses a closed interval [0, 1] on the edge parameter and removes duplicate t-values
/// (within G_EPSILON) that arise when the line passes exactly through a shared vertex.
///
/// @param P    A point on the infinite line.
/// @param dir  Direction of the infinite line (need not be unit length, but must be non-zero).
/// @param poly Closed polygon vertices (n unique vertices, not duplicated).
/// @return     Sorted, deduplicated list of t-values at which the line crosses polygon edges.
static std::vector<double> LinePolygonIntersections(
	const CorePt2 &P,
	const CoreVector2 &dir,
	const std::vector<CorePt2> &poly)
{
	std::vector<double> tValues;
	const size_t n = poly.size();

	for (size_t i = 0; i < n; ++i)
		{
		const CorePt2 &Q = poly[i];
		const CorePt2 &R = poly[(i + 1) % n];

		CoreVector2 e{ R.x - Q.x, R.y - Q.y };
		double denom = dir.x * e.y - dir.y * e.x;
		if (PARMZERO(denom))
			continue; // parallel – no unique intersection

		double dx = Q.x - P.x;
		double dy = Q.y - P.y;
		double t = (dx * e.y - dy * e.x) / denom;
		double s = (dx * dir.y - dy * dir.x) / denom;

		// Accept intersections within the closed interval [0, 1]
		if (GPARMGEZERO(s) && GPARMLE(s, 1.0))
			tValues.push_back(t);
		}

	std::sort(tValues.begin(), tValues.end());

	// Remove duplicate t-values that result from the line passing through a vertex
	// shared by two adjacent edges (each would contribute the same t value)
	tValues.erase(
		std::unique(tValues.begin(), tValues.end(), [](double a, double b)
		{
		return std::abs(b - a) <= G_EPSILON;
		}),
	tValues.end());

	return tValues;
}

/// Calculates the terminals of parallel lines within a clipped polygon region.
/// See header for full documentation.
std::vector<LineSegment> CalculateParallelLineTerminals(
	const std::vector<CorePt2> &polygon,
	size_t edgeIndex,
	double offsetDistance,
	double clippingDistance)
{
	std::vector<LineSegment> result;

	const size_t nVertices = polygon.size() - 1;
	if (nVertices < 3 || edgeIndex >= nVertices)
		return result;

	if (PARMLEZERO(offsetDistance) || PARMLEZERO(clippingDistance))
		return result;

	// Reference edge direction
	const CorePt2 &edgeStart = polygon[edgeIndex];
	const CorePt2 &edgeEnd   = polygon[edgeIndex + 1];
	CoreVector2 edgeDir{ edgeEnd.x - edgeStart.x, edgeEnd.y - edgeStart.y };
	edgeDir.Normalise();

	// Inward perpendicular (toward the polygon interior)
	bool ccw  = IsCounterClockwise(polygon);
	double sign = ccw ? 1.0 : -1.0;
	CoreVector2 perpDir{ -sign * edgeDir.y, sign * edgeDir.x };

	// Safety check: confirm perpDir actually points toward the centroid
	double cx = 0.0, cy = 0.0;
	for (int i = 0; i < nVertices; ++i)
		{
		cx += polygon[i].x;
		cy += polygon[i].y;
		}
	cx /= nVertices;
	cy /= nVertices;
	CoreVector2 toCentroid{ cx - edgeStart.x, cy - edgeStart.y };
	if (toCentroid.DotProduct(perpDir) < 0.0)
		perpDir = -perpDir;

	// Build the inset (yellow) clipping polygon
	std::vector<CorePt2> insetPoly = ComputeInsetPolygon(polygon, clippingDistance);
	if (insetPoly.empty())
		return result;

	// Determine the maximum perpendicular projection of the inset polygon
	// vertices onto perpDir relative to edgeStart – this bounds the iteration
	double maxProj = 0.0;
	for (const CorePt2 &pt : insetPoly)
		{
		double proj = (pt.x - edgeStart.x) * perpDir.x +
									(pt.y - edgeStart.y) * perpDir.y;
		if (proj > maxProj)
			maxProj = proj;
		}

	// Generate parallel lines at n * offsetDistance from the reference edge,
	// stepping inward, and clip each against the inset polygon
	for (int n = 1; n * offsetDistance <= maxProj + G_EPSILON; ++n)
		{
		CorePt2 linePoint
			{
			edgeStart.x + n *offsetDistance * perpDir.x,
			edgeStart.y + n *offsetDistance *perpDir.y
			};

		std::vector<double> tVals = LinePolygonIntersections(linePoint, edgeDir, insetPoly);

		// Pair up sorted intersection t-values to form line segments
		for (size_t i = 0; i + 1 < tVals.size(); i += 2)
			{
			CorePt2 segStart
				{
				linePoint.x + tVals[i]      *edgeDir.x,
				linePoint.y + tVals[i]      *edgeDir.y
				};
			CorePt2 segEnd
				{
				linePoint.x + tVals[i + 1] *edgeDir.x,
				linePoint.y + tVals[i + 1] *edgeDir.y
				};
			result.emplace_back(segStart, segEnd);
			}
		}

	return result;
}
