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


bool MakeSVG(const std::string &filename, const CImageData &imageData)
{
	svg::ShapeColl elements;

	for (const auto &polygon : imageData.polygons)
		{
		svg::Polygon poly{ svg::Fill{svg::Color{ 144, 238, 144 }}, svg::Stroke{POLY_STROKE, svg::Color::Green} };
		for (const auto &pt : polygon.points)
			{
			poly << svg::Point{ pt.x, pt.y };
			}
		elements << poly;
		}

	for (const auto &line : imageData.lines)
		{
		svg::Point p1(line.start.x, line.start.y);
		svg::Point p2(line.end.x, line.end.y);
		elements << svg::Line{ p1, p2, svg::Stroke{LINE_STROKE, svg::Color::Black} };
		}

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
		poly << svg::AnimateTransform{ svg::Point{ 0,0 }, movement, "1s" };
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

	if (doc.save())
		return true;
	return false;
}

bool MakeSVG(std::string &byteData, const CImageData &imageData)
{
	svg::ShapeColl elements;

	for (const auto &polygon : imageData.polygons)
		{
		svg::Polygon poly{ svg::Fill{svg::Color{ 144, 238, 144 }}, svg::Stroke{POLY_STROKE, svg::Color::Green} };
		for (const auto &pt : polygon.points)
			{
			poly << svg::Point{ pt.x, pt.y };
			}
		elements << poly;
		}

	for (const auto &line : imageData.lines)
		{
		svg::Point p1(line.start.x, line.start.y);
		svg::Point p2(line.end.x, line.end.y);
		elements << svg::Line{ p1, p2, svg::Stroke{LINE_STROKE, svg::Color::Black} };
		}

	for (const auto &arrow : imageData.arrows)
		{
		svg::Polygon poly{ svg::Fill{svg::Color{ 238, 144, 144 }}, svg::Stroke{ARROW_STROKE, svg::Color::Red} };
		for (const auto &pt : arrow.points)
			{
			poly << svg::Point{ pt.x, pt.y };
			}
		poly << svg::AnimateTransform{ svg::Point{ 0,0 }, svg::Point{ 10,10 }, "1s" };
		elements << poly;
		}

	svg::Layout layout;
	layout.dimensions = svg::Dimensions{ imageData.maximum.x - imageData.minimum.x, imageData.maximum.y - imageData.minimum.y };
	layout.origin_offset = svg::Point{ -imageData.minimum.x, -imageData.minimum.y };
	svg::Document doc("", layout);

	svg::Polygon background(svg::Fill{ svg::Color{ 211, 211, 211 } });
	background
			<< svg::Point(0, 0)
			<< svg::Point(layout.dimensions.width, 0)
			<< svg::Point(layout.dimensions.width, layout.dimensions.height)
			<< svg::Point(0, layout.dimensions.height);

	doc << background;
	doc << elements;
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

	std::vector<CorePt3> linePath =
		{
			{ 50010.0, 60010.0, 0 },
			{ 50030.0, 60030.0, 0 },
			{ 50030.0, 60050.0, 0 },
			{ 50050.0, 60070.0, 0 },
		};

	CParkingLayout layout;
	layout.Make(linePath, params);
	CImageData imageData = layout.GetImageData(params);

	return MakeSVG(filename, imageData);
}

bool parking_layout_svg_bytes(std::string &byteData, const std::string &wktGeometry, const std::string &jsonParams)
{
	ParkingParams params;
	DeserializeParams(jsonParams, params);

	std::vector<CorePt3> linePath =
		{
			{ 50010.0, 60010.0, 0 },
			{ 50030.0, 60030.0, 0 },
			{ 50030.0, 60050.0, 0 },
			{ 50050.0, 60070.0, 0 },
		};

	CParkingLayout layout;
	layout.Make(linePath, params);
	CImageData imageData = layout.GetImageData(params);

	return MakeSVG(byteData, imageData);
}
