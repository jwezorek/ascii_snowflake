#pragma once

#include "snowflake.hpp"
#include <string>

namespace asf {

    void display_title();
    void report_error(const std::string& msg = {});
    settings load_settings_from_file(const std::string& path);
    void print_settings(const asf::settings& s);
    void seed_rand_generator(unsigned int seed);
    int random_int(int n);
    bool random_chance(double p);

    template<typename T>
    inline T random_element(const std::vector<T>& v) {
        return v.at(random_int(static_cast<int>(v.size())));
    }

}