#include <print>
#include "snowflake.hpp"
#include "util.hpp"

int main(int argc, char* args[]) {
	using namespace asf;

	try {
		if (argc < 2 || argc > 3) {
			report_error("usage is like 'ascii-snowflake.exe settings.json [rand seed]");
			return -1;
		}
		auto settings = asf::load_settings_from_file(args[1]);

		display_title();
		std::println("    generating snowflakes with\n");

		if (argc == 3) {
			unsigned int seed = std::stoi(args[2]);
			seed_rand_generator(seed);
			std::println("    rand seed: {}", seed);
		}

		print_settings(settings);
		std::println("");

		auto snowflakes = asf::grow_snowflakes(settings);
		for (const auto& snowflake : snowflakes) {
			display(snowflake);
			std::println("");
		}
	}
	catch (std::runtime_error err) {
		report_error(err.what());
		return -1;
	} catch (...) {
		report_error();
		return -1;
	}
	return 0;
}


