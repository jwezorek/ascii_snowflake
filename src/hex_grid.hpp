#pragma once

#include <ranges>
#include "vec.hpp"

/*------------------------------------------------------------------------------------------------*/

namespace asf {

    using hex_coords = vec3<int>;
    using hex_grid = vec3_map<int, int>;
    using hex_set = vec3_set<int>;

    void display(const hex_grid& grid);
    hex_grid rotate(const hex_grid& grid, int num_sixths);
    hex_coords flip_horz(const hex_coords& hex);
    hex_grid flip_horz(const hex_grid& grid);
    hex_grid union_(const hex_grid& g1, const hex_grid& g2);
    int distance(const hex_coords& a, const hex_coords& b);

    inline auto neighbors(const hex_coords& hex, bool with_diagonals) {
        namespace rv = std::ranges::views;
        static std::vector<hex_coords> direct_offsets = { {
            {+1, 0, -1},  {+1, -1, 0},  {0, -1, +1},
            {-1, 0, +1},  {-1, +1, 0},  {0, +1, -1},
        } };
        static std::vector<hex_coords> offsets_with_diagonals = { {
            {+1, 0, -1},  {+1, -1, 0},  {0, -1, +1},
            {-1, 0, +1},  {-1, +1, 0},  {0, +1, -1},
            {+2, -1, -1}, {+1, -2, +1}, {-1, -1, +2},
            {-2, +1, +1}, {-1, +2, -1}, {+1, +1, -2},
        } };

        auto offsets = (with_diagonals) ?
            rv::all(offsets_with_diagonals) :
            rv::all(direct_offsets);

        return offsets | rv::transform(
            [hex](auto&& offset) {
                return hex + offset;
            }
        );
    }

    inline auto hex_region(int radius) {
        namespace rv = std::ranges::views;
        return rv::cartesian_product(
            rv::iota( -radius, radius + 1 ),
            rv::iota( -radius, radius + 1 ),
            rv::iota( -radius, radius + 1 )
        ) | rv::filter(
            [](auto&& coords) {
                const auto& [q, r, s] = coords;
                return q + r + s == 0;
            }
        ) | rv::transform(
            [](auto&& coords)->hex_coords {
                const auto& [q, r, s] = coords;
                return { q,r,s };
            }
        );
    }

    inline auto tri_region(int radius) {
        namespace rv = std::ranges::views;
        return rv::cartesian_product(
            rv::iota(0, radius + 1),
            rv::iota(-radius, 1)
        ) | rv::filter(
            [](auto&& coords) {
                const auto& [q, r] = coords;
                return q + r <= 0;
            }
        ) | rv::transform(
            [](auto&& coords)->hex_coords {
                const auto& [q, r] = coords;
                return { q,r, -q - r };
            }
        );
    }

}