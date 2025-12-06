// console.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "../parking/ParkingImage.h"

int main(int argc, char *argv[])
	{
	if (argc == 1)
		{
		std::string buffer;

		if (parking_layout_svg_bytes(buffer))
			std::cout << buffer << std::endl;
		else
			std::cout << "Failed" << std::endl;
		}
	else
		{
		std::string filename{ argv[1] };
		filename += ".svg";

		if (parking_layout_svg_file(filename))
			std::cout << filename << " written" << std::endl;
		else
			std::cout << "Failed" << std::endl;
		}
}
