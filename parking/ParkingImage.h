#pragma once
#include <string>

bool parking_layout_svg_file(const std::string &filename, const std::string &wktGeometry = {}, const std::string &jsonParams = {});
bool parking_layout_svg_bytes(std::string &byteData, const std::string &wktGeometry = {}, const std::string &jsonParams = {});
