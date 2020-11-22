#include "clishell.h"
#include "CLI11.hpp"

int CAparser(int argc, char** argv, std::string& simulator, unsigned& particle_number, unsigned& stones_size, bool& isFullscreen, unsigned& window_width, unsigned& window_height)
{
	CLI::App app("ParticleSimulator");

	app.add_option("-s,--simulator", simulator, "Choose simulator (waterfall/fire/sand/net)")
		->required(true);
	app.add_option("-n,--number", particle_number, "Number of particles")
		->required(true);
	app.add_option("-o,--obstacles", stones_size, "Stones' size for waterfall");
	app.add_flag("-f,--isFullscreen", isFullscreen, "Is window fullscreen mode?");
	app.add_option("--width", window_width, "Window's width");
	app.add_option("--height", window_height, "Window's height");
	std::cout << "Here" << std::endl;

	CLI11_PARSE(app, argc, argv);
	return 1;
}
