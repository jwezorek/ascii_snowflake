#include "hex_grid.hpp"
#include <print>
#include <algorithm>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    using squ_coords = asf::vec2<int>;
    using squ_grid = asf::vec2_map<int, char>;

    constexpr int k_max_states = 10;
    constexpr std::array<char, k_max_states> k_open_tiles = { {
        ' ','[','(','{','<','o','O','0','*','#'
    } };
    constexpr std::array<char, k_max_states> k_close_tiles = { {
        ' ',']',')','}','>','o','O','0','*','#'
    } };

    squ_coords to_squ_coords(const asf::hex_coords& hex) {
        int x = hex.x - hex.z;
        int y = hex.x + hex.z;
        return { x,y };
    }

    squ_grid to_squ_grid(const asf::hex_grid& grid) {
        return grid | rv::transform(
            [](auto&& cell) {
                return rv::iota(0, 2) | rv::transform(
                    [cell](auto i)->squ_grid::value_type {
                        const auto [hex, val] = cell;
                        char tile = (i == 0) ? k_open_tiles[val] : k_close_tiles[val];
                        auto loc = to_squ_coords(hex) + squ_coords{ i,0 };
                        return { loc, tile };
                    }
                );
            }
        ) | rv::join | r::to<squ_grid>();
    }

    asf::hex_coords rotate(const asf::hex_coords& hex, int n) {
        auto sign = (n % 2 == 0) ? 1 : -1;
        std::array<int, 3> ary = { {hex.x,hex.y,hex.z} };
        return {
            sign * ary[(0 + n) % 3],
            sign * ary[(1 + n) % 3],
            sign * ary[(2 + n) % 3]
        };
    }

}

void asf::display(const asf::hex_grid& hexagons) {
    auto grid = to_squ_grid(hexagons);
    if (grid.empty()) {
        return;
    }

    auto [x1, x2] = r::minmax(
        grid | rv::transform([](auto&& itm) {return itm.first.x; })
    );
    auto [y1, y2] = r::minmax(
        grid | rv::transform([](auto&& itm) {return itm.first.y; })
    );
    for (auto y = y1; y <= y2; ++y) {
        std::print("  ");
        for (auto x = x1; x <= x2; ++x) {
            char tile = grid.contains({ x,y }) ? grid.at({ x,y }) : ' ';
            std::print("{}", tile);
        }
        std::println("");
    }
}

asf::hex_grid asf::rotate(const hex_grid& grid, int num_sixths) {
    return grid | rv::transform(
        [num_sixths](auto&& itm)->hex_grid::value_type {
            const auto& [hex, val] = itm;
            return { ::rotate(hex, num_sixths), val };
        }
    ) | r::to<hex_grid>();
}

asf::hex_coords asf::flip_horz(const hex_coords& hex) {
    auto flipped = hex;
    std::swap(flipped.x, flipped.z);
    return flipped;
}

asf::hex_grid asf::flip_horz(const hex_grid& grid) {
    return grid | rv::transform(
        [](auto&& itm)->hex_grid::value_type {
            const auto& [hex, val] = itm;
            return { flip_horz(hex), val };
        }
    ) | r::to<hex_grid>();
}

asf::hex_grid asf::union_(const hex_grid& g1, const hex_grid& g2) {
    hex_grid out;
    r::copy(g1, std::inserter(out, out.end()));
    r::copy(g2, std::inserter(out, out.end()));
    return out;
}

int asf::distance(const hex_coords& a, const hex_coords& b) {
    auto diff = a - b;
    return (std::abs(diff.x) + std::abs(diff.y) + std::abs(diff.z)) / 2;
}
