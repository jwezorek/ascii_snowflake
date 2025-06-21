#include "util.hpp"
#include "third-party/json.hpp"
#include <random>
#include <fstream>
#include <stdexcept>
#include <print>

namespace {

    static std::random_device g_rd;
    static std::mt19937 g_gen(g_rd());
}

int asf::random_int(int n) {
    std::uniform_int_distribution<int> dist(0, n - 1);
    return dist(g_gen);
}

bool asf::random_chance(double p) {
    std::bernoulli_distribution dist(p);
    return dist(g_gen);
}

void asf::display_title() {
     std::println(R"(             
                   (_|_)                          
      __ _ ___  ___ _ _                           
     / _` / __|/ __| | |      __ _       _        
    | (_| \__ \ (__| | |     / _| |     | |       
     \__,_|___/\___|_|_|   _| |_| | __ _| | _____ 
    / __| '_ \ / _ \ \ /\ / /  _| |/ _` | |/ / _ \
    \__ \ | | | (_) \ V  V /| | | | (_| |   <  __/
    |___/_| |_|\___/ \_/\_/ |_| |_|\__,_|_|\_\___|

    by Joe Wezorek...

    )" );
}

void asf::report_error(const std::string& msg) {
    auto message = msg;
    if (message.empty()) {
        message = "unknown";
    }
    std::println("  error: {}", message);
}

asf::settings asf::load_settings_from_file(const std::string& path) {
	std::ifstream file(path);
	if (!file) {
		throw std::runtime_error("could not open file: " + path);
	}

    asf::settings s;
    try {
        nlohmann::json j;
        file >> j;

        s.population_sz = j.at("population_sz").get<int>();
        s.num_children = j.at("num_children").get<int>();
        s.num_states = j.at("num_states").get<int>();
        s.primordial_soup_density = j.at("primordial_soup_density").get<double>();
        s.primordial_soup_radius = j.at("primordial_soup_radius").get<int>();
        s.state_table_density = j.at("state_table_density").get<double>();
        s.max_generations = j.at("max_generations").get<int>();
        s.tries_per_generation = j.at("tries_per_generation").get<int>();
        s.num_iterations = j.at("num_iterations").get<int>();
        s.num_output_snowflakes = j.at("num_output_snowflakes").get<int>();

        const auto& sp = j.at("score_params");
        s.score_params.connectedness_weight = sp.at("connectedness_weight").get<double>();
        s.score_params.airiness_weight = sp.at("airiness_weight").get<double>();
        s.score_params.spikiness_weight = sp.at("spikiness_weight").get<double>();
        s.score_params.cragginess_weight = sp.at("cragginess_weight").get<double>();
        s.score_params.max_density = sp.at("max_density").get<double>();
        s.score_params.max_radius = sp.at("max_radius").get<int>();
        s.score_params.min_radius = sp.at("min_radius").get<int>();
    } catch (...) {
        throw std::runtime_error("bad JSON settings.");
    }
	return s;
}

void asf::print_settings(const asf::settings& s) {
    using namespace std;

    println("    snowflake settings: {{");
    println("      population_sz: {}", s.population_sz);
    println("      num_children: {}", s.num_children);
    println("      num_states: {}", s.num_states);
    println("      primordial_soup_density: {}", s.primordial_soup_density);
    println("      primordial_soup_radius: {}", s.primordial_soup_radius);
    println("      state_table_density: {}", s.state_table_density);
    println("      max_generations: {}", s.max_generations);
    println("      tries_per_generation: {}", s.tries_per_generation);
    println("      num_iterations: {}", s.num_iterations);
    println("      num_output_snowflakes: {}", s.num_output_snowflakes);

    const auto& p = s.score_params;
    println("      score parameters: {{");
    println("        connectedness_weight: {}", p.connectedness_weight);
    println("        airiness_weight: {}", p.airiness_weight);
    println("        spikiness_weight: {}", p.spikiness_weight);
    println("        cragginess_weight: {}", p.cragginess_weight);
    println("        max_density: {}", p.max_density);
    println("        max_radius: {}", p.max_radius);
    println("        min_radius: {}", p.min_radius);
    println("      }}");
    println("    }}");
}

void asf::seed_rand_generator(unsigned int seed) {
	g_gen = std::mt19937{ seed };
}

