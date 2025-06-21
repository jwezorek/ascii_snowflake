#include "snowflake.hpp"
#include "util.hpp"
#include <random>
#include <ranges>
#include <stack>
#include <map>
#include <tuple>
#include <print>
#include <execution>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    constexpr double k_connected_by_diagonals_score = 0.5;


    using state_table = std::vector<std::vector<int>>;

    std::tuple<size_t, size_t> dimensions(const state_table& tbl) {
        return {
            tbl.at(0).size(),
            tbl.size()
        };
    }

    state_table empty_state_table(int cols, int rows) {
        return rv::iota(
            0, rows
        ) | rv::transform(
            [&](auto) {
                return std::vector<int>(cols, 0);
            }
        ) | r::to<std::vector>();
    }

    state_table mix_state_tables(const state_table& tbl1, const state_table& tbl2) {
        auto [cols, rows] = dimensions(tbl1);
        auto child = empty_state_table(cols, rows);
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                child[row][col] = asf::random_chance(0.5) ? tbl1[row][col] : tbl2[row][col];
            }
        }
        return child;
    }

    asf::hex_set active_cells(const asf::hex_grid& grid) {
        auto active = grid | rv::keys | rv::transform(
            [](auto&& hex) {
                return asf::neighbors(hex, false);
            }
        ) | rv::join | r::to<asf::hex_set>();
        r::copy(grid | rv::keys, std::inserter(active, active.end()));
        return active;
    }

    int state_at(const asf::hex_grid& grid, const asf::hex_coords& hex) {
        if (!grid.contains(hex)) {
            return 0;
        }
        return grid.at(hex);
    }

    int neighbor_sum(const asf::hex_grid& grid, const asf::hex_coords& hex) {
        return r::fold_left(
            asf::neighbors(hex, false) | rv::transform(
                [&](auto&& neighbor) {
                    return state_at(grid, neighbor);
                }
            ),
            0,
            std::plus<>()
        );
    }

    int neighbor_count(const asf::hex_grid& grid, const asf::hex_coords& hex) {
        return r::fold_left(
            asf::neighbors(hex, false) | rv::transform(
                [&](auto&& neighbor) {
                    return grid.contains(neighbor) ? 1 : 0;
                }
            ),
            0,
            std::plus<>()
        );
    }

    template<typename Grid>
    bool is_connected(const Grid& grid, bool with_diagonals) {
        if (grid.empty()) return true;
        asf::hex_set visited;
        std::stack<asf::hex_coords> stack;

        // Extract initial key
        if constexpr (requires { grid.begin()->first; }) {
            stack.push(grid.begin()->first); // unordered_map
        } else {
            stack.push(*grid.begin()); // unordered_set
        }

        while (!stack.empty()) {
            auto current = stack.top();
            stack.pop();

            if (visited.contains(current)) continue;
            visited.insert(current);

            for (auto neighbor : asf::neighbors(current, with_diagonals)) {
                if (!grid.contains(neighbor)) continue;
                stack.push(neighbor);
            }
        }
        return visited.size() == grid.size();
    }

    int max_radius(const asf::hex_grid& grid) {
        if (grid.empty()) {
            return 0;
        }
        auto distance_from_origin = [](const asf::hex_coords& hex) {
            return asf::distance(hex, { 0,0,0 });
            };
        return r::max(
            grid | rv::keys | rv::transform(distance_from_origin)
        );
    }

    double snowflake_airiness(const asf::hex_grid& grid, int radius) {
        int total_count = 0;
        int air_count = 0;
        for (const auto& hex : asf::hex_region(radius)) {
            total_count++;
            air_count += (!grid.contains(hex)) ? 1 : 0;
        }
        return static_cast<double>(air_count) / static_cast<double>(total_count);
    }

    double snowflake_cragginess(const asf::hex_grid& grid, int radius) {
        int periphery_cells = 0;
        int high_neighbor_cells = 0;
        for (auto hex : active_cells(grid)) {
            if (grid.contains(hex)) {
                continue;
            }
            periphery_cells++;
            if (neighbor_count(grid, hex) >= 3) {
                auto neighbor_set = asf::neighbors(hex, false) | rv::filter(
                    [&](auto&& hex) {
                        return grid.contains(hex);
                    }
                ) | r::to<asf::hex_set>();
                if (is_connected(neighbor_set, false)) {
                    high_neighbor_cells++;
                }
            }
        }
        return static_cast<double>(high_neighbor_cells) / static_cast<double>(periphery_cells);
    }

    double snowflake_spikiness(const asf::hex_grid& grid, int radius) {
        int total_alive = 0;
        double alive_edge_proximity = 0.0;

        for (const auto hex : asf::tri_region(radius)) {
            if (!grid.contains(hex)) continue;

            int row = -hex.y;
            double edge_proximity = 1.0;

            if (row > 0) {
                double max_dist = static_cast<double>(row) / 2.0;
                double dist = static_cast<double>(
                    std::min(std::abs(hex.x), std::abs(hex.x - row))
                    );
                auto relative_edge_dist = dist / max_dist;
                edge_proximity = 1.0 - std::pow(relative_edge_dist, 2.0);
            }

            alive_edge_proximity += edge_proximity;
            ++total_alive;
        }

        return total_alive > 0
            ? alive_edge_proximity / static_cast<double>(total_alive)
            : 0.0;
    }

    struct snowflake_info {
        asf::hex_grid snowflake;
        double score;
        state_table tbl;
    };

    asf::hex_grid random_initial_grid(double density, int num_states, int radius) {
        asf::hex_set visited;
        asf::hex_grid tri;
        for (auto hex : asf::tri_region(radius)) {
            if (visited.contains(hex)) {
                continue;
            }
            auto flipped = asf::flip_horz(hex);
            if (asf::random_chance(density)) {
                tri[hex] = 1 + asf::random_int(num_states - 1);
                tri[flipped] = tri[hex];
            }
            visited.insert(hex);
            visited.insert(flipped);
        }
        asf::hex_grid out;
        for (int i = 0; i < 6; ++i) {
            out = asf::union_(out, asf::rotate(tri, i));
        }
        return out;
    }

    state_table random_state_table(double alive_prob, int num_states) {
        int max_sum = 6 * num_states;
        return rv::iota(
            0, num_states
        ) | rv::transform(
            [&](auto) {
                return rv::iota(0, max_sum + 1) | rv::transform(
                    [&](auto) {
                        if (asf::random_chance(1.0 - alive_prob)) {
                            return 0;
                        }
                        return 1 + asf::random_int(num_states - 1);
                    }
                ) | r::to<std::vector>();
            }
        ) | r::to<std::vector>();
    }

    asf::hex_grid do_cellular_automata_step(const asf::hex_grid& current, const state_table& tbl) {
        asf::hex_grid next;
        for (auto hex : active_cells(current)) {
            auto sum = neighbor_sum(current, hex);
            auto next_state = tbl[state_at(current, hex)][sum];
            if (next_state > 0) {
                next[hex] = next_state;
            }
        }
        return next;
    }

    double score_snowflake(const asf::hex_grid& grid, const asf::snowflake_metric_params& params) {
        double connectedness = 0.0;
        if (is_connected(grid, false)) {
            connectedness = 1.0;
        } else if (is_connected(grid, true)) {
            connectedness = k_connected_by_diagonals_score;
        }

        if (connectedness == 0.0) {
            return 0.0;
        }

        auto radius = max_radius(grid);
        if (radius < params.min_radius || radius > params.max_radius) {
            return 0;
        }

        auto airiness = snowflake_airiness(grid, radius);
        auto density = 1.0 - airiness;
        if (density < params.min_density || density > params.max_density) {
            return 0.0;
        }

        auto cragginess = snowflake_cragginess(grid, radius);
        auto spikiness = snowflake_spikiness(grid, radius);

        return params.connectedness_weight * connectedness +
            params.airiness_weight * airiness +
            params.cragginess_weight * cragginess +
            params.spikiness_weight * spikiness;
    }

    snowflake_info generate_snowflake(
            const asf::hex_grid& initial_configuration, const state_table& tbl, 
            const asf::settings& settings) {
        auto state = initial_configuration;
        for (int i = 0; i < settings.num_iterations; ++i) {
            state = do_cellular_automata_step(state, tbl);
        }
        return { state, score_snowflake(state, settings.score_params), tbl };
    }

    double mean_score(const std::vector<snowflake_info>& snowflakes) {
        auto sum = r::fold_left(
            snowflakes | rv::transform(
                [](auto&& snowflake) {
                    return snowflake.score;
                }
            ),
            0.0,
            std::plus<>()
        );
        return sum / snowflakes.size();
    }

    std::vector<snowflake_info> do_next_generation(
        const std::vector<state_table>& population,
        const asf::settings& settings,
        double last_score) {

        double score = 0.0;
        std::vector<snowflake_info> snowflakes;

        int tries = 0;
        while (score <= last_score && tries < settings.tries_per_generation) {
            std::print(".");

            std::vector<std::tuple<state_table, asf::hex_grid>> work_items;
            for (int child = 0; child < settings.num_children; ++child) {
                work_items.emplace_back(
                    mix_state_tables(
                        asf::random_element(population), asf::random_element(population)
                    ),
                    random_initial_grid(
                        settings.primordial_soup_density,
                        settings.num_states,
                        settings.primordial_soup_radius
                    )
                );
            }

            snowflakes.resize(work_items.size());
            std::transform(
                std::execution::par,
                work_items.begin(),
                work_items.end(),
                snowflakes.begin(),
                [&](const auto& work_item) {
                    const auto& [tbl, seed] = work_item;
                    return generate_snowflake(seed, tbl, settings);
                }
            );

            r::sort(snowflakes,
                [](const snowflake_info& lhs, const snowflake_info& rhs) {
                    return lhs.score > rhs.score;
                }
            );
            snowflakes = snowflakes | rv::take(settings.population_sz) | r::to<std::vector>();

            score = mean_score(snowflakes); 
            ++tries;
        }

        std::println("");
        return (score > last_score) ? snowflakes : std::vector<snowflake_info>{};
    }
}

std::vector<asf::hex_grid> asf::grow_snowflakes(const settings& settings) {
    auto population = rv::iota(0, settings.population_sz) | rv::transform(
            [&](auto) {
                return random_state_table(settings.state_table_density, settings.num_states);
            }
        ) | r::to<std::vector>();

    double last_score = 0;
    std::vector<snowflake_info> snowflakes;
    for (int gen = 0; gen < settings.max_generations; ++gen) {
        std::print("    generation {}", gen + 1);
        auto next_gen = do_next_generation(population, settings, last_score);
        if (next_gen.empty()) {
            std::println("      no improvement in {} tries", settings.tries_per_generation);
            break;
        }
        snowflakes = next_gen;
        last_score = mean_score(snowflakes);
        population = snowflakes | rv::transform(
            [](auto&& sf_info) {
                return sf_info.tbl;
            }
        ) | r::to<std::vector>();
        std::println("      o mean score: {}", last_score);
    }

    return snowflakes | rv::take(
            settings.num_output_snowflakes
        ) | rv::transform(
            [](auto&& sf_info) {
                return sf_info.snowflake;
            }
        ) | r::to<std::vector>();

}


