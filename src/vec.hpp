#pragma once

#include <unordered_set>
#include <unordered_map>

/*------------------------------------------------------------------------------------------------*/

namespace asf {

    template<typename T>
    struct vec2 {
        T x;
        T y;

        bool operator==(const vec2<T>& rhs) const {
            return x == rhs.x && y == rhs.y;
        }
    };

    template<typename T>
    vec2<T> operator+(const vec2<T>& lhs, const vec2<T>& rhs) {
        return {
            lhs.x + rhs.x,
            lhs.y + rhs.y
        };
    }

    template<typename T>
    vec2<T> operator-(const vec2<T>& lhs, const vec2<T>& rhs) {
        return {
            lhs.x - rhs.x,
            lhs.y - rhs.y
        };
    }

    template<typename T>
    vec2<T> operator*(T lhs, const vec2<T>& rhs) {
        return {
            lhs * rhs.x,
            lhs * rhs.y
        };
    }

    template <typename T>
    inline void hash_combine(std::size_t& seed, const T& val) {
        std::hash<T> hasher;
        seed ^= hasher(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    template<typename T>
    struct hash_vec2 {
        size_t operator()(const vec2<T>& pt) const {
            size_t seed = 0;
            hash_combine(seed, pt.x);
            hash_combine(seed, pt.y);
            return seed;
        }
    };

    template<typename T>
    using vec2_set = std::unordered_set<vec2<T>, hash_vec2<T>>;

    template<typename T, typename U>
    using vec2_map = std::unordered_map<vec2<T>, U, hash_vec2<T>>;

    /*------------------------------------------------------------------------------------------------*/

    template<typename T>
    struct vec3 {
        T x;
        T y;
        T z;

        bool operator==(const vec3<T>& rhs) const {
            return x == rhs.x && y == rhs.y && z == rhs.z;
        }
    };

    template<typename T>
    vec3<T> operator+(const vec3<T>& lhs, const vec3<T>& rhs) {
        return {
            lhs.x + rhs.x,
            lhs.y + rhs.y,
            lhs.z + rhs.z,
        };
    }

    template<typename T>
    vec3<T> operator-(const vec3<T>& lhs, const vec3<T>& rhs) {
        return {
            lhs.x - rhs.x,
            lhs.y - rhs.y,
            lhs.z - rhs.z
        };
    }

    template<typename T>
    vec3<T> operator*(T lhs, const vec3<T>& rhs) {
        return {
            lhs * rhs.x,
            lhs * rhs.y,
            lhs * rhs.z
        };
    }

    template<typename T>
    struct hash_vec3 {
        size_t operator()(const vec3<T>& pt) const {
            size_t seed = 0;
            hash_combine(seed, pt.x);
            hash_combine(seed, pt.y);
            hash_combine(seed, pt.z);
            return seed;
        }
    };

    template<typename T>
    using vec3_set = std::unordered_set<vec3<T>, hash_vec3<T>>;

    template<typename T, typename U>
    using vec3_map = std::unordered_map<vec3<T>, U, hash_vec3<T>>;

}