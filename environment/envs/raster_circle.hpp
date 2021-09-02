#pragma once

namespace agario::env {

/*
Class for iterating over the point in a rasterized circle.

Example:

    int center_x = 2;
    int center_y = 3;
    float radius = 10.3;
    for (const auto [dx, dy] : RasterCircle(radius)) {
        data[center_x + dx][center_y + dy] = 1.0;
    }
*/
template<typename T=int>
class RasterCircle {
    private:
        float radius_;
    public:

    RasterCircle(float radius) : radius_(radius) {}
    
    class iterator {
    public:
        using value_type = std::pair<T, T>;
        using pointer = const value_type*;
        using reference = const value_type&;
        using iterator_category = std::forward_iterator_tag;

        iterator(float radius, T x, T y, float err, int quadrant) :
         radius(radius), x(x), y(y), err(err), quadrant(quadrant) {}

        iterator& operator++() {
            if (quadrant < 0) return *this;
            if (quadrant < 3) {
                quadrant++;
            } else {
                quadrant = 0;
                float r = err;
                if (r > x) {
                    x++;
                    err += x * 2 + 1;
                }
                if (r <= y) {
                    y++;
                    err += y * 2 + 1;
                }
                if (x >= 0) {
                    quadrant = -1;
                }
            }
            return *this;
        }
        iterator operator++(int) { iterator retval = *this; ++(*this); return retval; }
        bool operator==(iterator other) const {
            if (quadrant < 0) return other.quadrant < 0;
            return (
                quadrant == other.quadrant &&
                x == other.x && y == other.y &&
                radius == other.radius
            );
        }
        bool operator!=(iterator other) const { return !(*this == other); }
        value_type operator*() {
            switch (quadrant) {
                case 0: return {x, y};
                case 1: return {-x, y};
                case 2: return {x, -y};
                case 3: return {-x, -y};
                default: break;
            }
            return {0, 0};
        }
    private:
        float radius;
        T x, y;
        float err;
        int quadrant;
    };
    iterator begin() {
        return iterator {
            /*radius=*/radius_,
            /*x=*/-static_cast<T>(radius_),
            /*y=*/0,
            /*err=*/2 - 2 * radius_,
            /*quadrant=*/0
        };
    }
    iterator end() { return iterator{0, 0, 0, 0, -1}; }
};


// void minsky_circle(int radius) {
//     int x = radius;
//     int y = 0;
//     bool past_halfway = false;
//     bool completed = false;
//     while (!completed) {
//         x += y >> 4;
//         y -= x >> 4;
//         // yield x, y;
//         past_halfway |= x < 0 && y < 0;
//         completed = past_halfway && x >= 0 && y >= 0;
//     }
//     // done
// }


} // namespace agario::env 
