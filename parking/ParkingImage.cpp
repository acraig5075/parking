// parking.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"
#include "ParkingImage.h"
#include "ParkingLayout.h"
#include "simple_svg_1.0.0.hpp"
#include <nlohmann/json.hpp>

constexpr double POLY_STROKE = 2.5;
constexpr double LINE_STROKE = 1.0;
constexpr double ARROW_STROKE = 2.5;
constexpr double PATH_STROKE = 3.0;


bool MakeSVG(const CImageData &imageData, const std::string &filename, std::string &byteData)
{
	svg::ShapeColl elements;

	// Caps
	for (const auto &polygon : imageData.polygons)
		{
		svg::Polygon poly{ svg::Fill{svg::Color{ 144, 238, 144 }}, svg::Stroke{POLY_STROKE, svg::Color::Green} };
		for (const auto &pt : polygon.points)
			{
			poly << svg::Point{ pt.x, pt.y };
			}
		elements << poly;
		}

	// Bays
	for (const auto &line : imageData.lines)
		{
		svg::Point p1(line.start.x, line.start.y);
		svg::Point p2(line.end.x, line.end.y);
		elements << svg::Line{ p1, p2, svg::Stroke{LINE_STROKE, svg::Color::Black} };
		}

	// Paths
	for (const auto &line : imageData.paths)
		{
		svg::Point p1(line.start.x, line.start.y);
		svg::Point p2(line.end.x, line.end.y);
		elements << svg::Line{ p1, p2, svg::Stroke{PATH_STROKE, svg::Color::Blue} };
		}

	// Arrows
	for (const auto &arrow : imageData.arrows)
		{
		svg::Polygon poly{ svg::Fill{svg::Color{ 238, 144, 144 }}, svg::Stroke{ARROW_STROKE, svg::Color::Red} };
		for (const auto &pt : arrow.points)
			{
			poly << svg::Point{ pt.x, pt.y };
			}
		svg::Point movement;
		movement.x = arrow.movement[1].x - arrow.movement[0].x;
		movement.y = arrow.movement[1].y - arrow.movement[0].y;
		poly << svg::AnimateTransform{ svg::Point{ 0, 0 }, movement, "1s" };
		elements << poly;
		}

	svg::Layout layout;
	layout.dimensions = svg::Dimensions{ imageData.maximum.x - imageData.minimum.x, imageData.maximum.y - imageData.minimum.y };
	layout.origin_offset = svg::Point{ -imageData.minimum.x, -imageData.minimum.y };
	svg::Document doc(filename, layout);

	svg::Polygon background(svg::Fill{ svg::Color{ 211, 211, 211 } });
	background
			<< svg::Point(0, 0)
			<< svg::Point(layout.dimensions.width, 0)
			<< svg::Point(layout.dimensions.width, layout.dimensions.height)
			<< svg::Point(0, layout.dimensions.height);

	doc << background;
	doc << elements;

	if (!filename.empty() && !doc.save())
		return false;

	byteData = doc.toString();
	return true;
}

void DeserializeParams(const std::string &jsonParams, ParkingParams &params)
{
	if (jsonParams.empty())
		return;

	nlohmann::json json = nlohmann::json::parse(jsonParams);

	params.driveDirection  = json["driveDirection"].template get<ParkingParams::DriveDirection>();
	params.rows            = json["rows"].template get <ParkingParams::LayoutRows>();
	params.sideLines       = json["sideLines"].template get <ParkingParams::PaintLines>();
	params.frontLine       = json["frontLine"].template get <ParkingParams::PaintLines>();
	params.rearLine        = json["rearLine"].template get <ParkingParams::PaintLines>();
	params.length          = json["length"].template get<double>();
	params.width           = json["width"].template get<double>();
	params.angle           = json["angle"].template get<double>();
	params.shortPaintLength = json["shortPaintLength"].template get<double>();
	params.capWidth        = json["capWidth"].template get<double>();
	params.capFrontExt     = json["capFrontExt"].template get<double>();
	params.showBays        = json["showBays"].template get<bool>();
	params.showCaps        = json["showCaps"].template get<bool>();
	params.showArrows      = json["showArrows"].template get<bool>();
	params.showPaths       = json["showPaths"].template get<bool>();
}

bool parking_layout_svg_file(const std::string &filename, const std::string &wktGeometry, const std::string &jsonParams)
{
	ParkingParams params;
	DeserializeParams(jsonParams, params);

	CParkingLayout layout;
	layout.Make(wktGeometry, params);

	CImageData imageData = layout.GetImageData(params);

	std::string unused;
	return MakeSVG(imageData, filename, unused);
}

bool parking_layout_svg_bytes(std::string &byteData, const std::string &wktGeometry, const std::string &jsonParams)
{
	ParkingParams params;
	DeserializeParams(jsonParams, params);

	CParkingLayout layout;
	layout.Make(wktGeometry, params);

	CImageData imageData = layout.GetImageData(params);

	std::string unused;
	return MakeSVG(imageData, unused, byteData);
}
