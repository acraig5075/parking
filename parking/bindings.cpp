#include <emscripten/bind.h>
#include "ParkingImage.h"

static std::string generate_svg(const std::string &wktGeometry, const std::string &jsonParams)
	{
	std::string result;
	parking_layout_svg_bytes(result, wktGeometry, jsonParams);
	return result;
	}

EMSCRIPTEN_BINDINGS(parking)
	{
	emscripten::function("generate_svg", &generate_svg);
	}
