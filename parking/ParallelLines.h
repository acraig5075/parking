#pragma once
#include "Core.h"
#include <vector>

struct LineSegment
{
	LineSegment() = default;
	LineSegment(const CorePt2 &start, const CorePt2 &end);

	CorePt2 start;
	CorePt2 end;
};

/// Calculates the terminals of parallel lines within a clipped polygon region.
/// The lines are parallel to the polygon edge at edgeIndex, spaced offsetDistance apart,
/// and clipped to the inset polygon that is offset inward by clippingDistance.
///
/// @param polygon          Corner coordinates of the boundary polygon. Start and end points are duplicated.
/// @param edgeIndex        Zero-based index into polygon for the reference edge (edge goes from polygon[edgeIndex] to polygon[edgeIndex+1]).
/// @param offsetDistance   Perpendicular spacing between consecutive parallel lines.
/// @param clippingDistance Inset distance used to derive the yellow clipping polygon.
/// @return                 Line segments whose endpoints lie on the inset polygon boundary.
std::vector<LineSegment> CalculateParallelLineTerminals(
	const std::vector<CorePt2> &polygon,
	size_t edgeIndex,
	double offsetDistance,
	double clippingDistance);
