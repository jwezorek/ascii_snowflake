#pragma once

#include "hex_grid.hpp"
#include <vector>

namespace asf {

    struct snowflake_metric_params {
        double connectedness_weight;
        double airiness_weight;
        double spikiness_weight;
        double cragginess_weight;
        double max_density;
        int    max_radius;
        int    min_radius;
    };

    struct settings {
        int population_sz;
        int num_children;
        int num_states;
        double primordial_soup_density;
        double primordial_soup_radius;
        double state_table_density;
        int max_generations;
        int tries_per_generation;
        int num_iterations;
        int num_output_snowflakes;
        snowflake_metric_params score_params;
    };

    std::vector<hex_grid> grow_snowflakes(const settings& settings);
}