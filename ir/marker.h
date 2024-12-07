#ifndef IR_MARKER_H
#define IR_MARKER_H

#include "utils/macros.h"

#include <cstdint>
#include <bitset>
#include <iostream>

namespace compiler {

using Marker = uint32_t;

// Marker structure:
//
// | unique       |         |
// | marker index |  color  |
// |______________|_________|
//     30 bits      2 bits

class MarkerManager final {
public:
    static constexpr uint32_t COLOR_BITS = 2;
    static constexpr uint32_t MARKER_INDEX_BITS = 30;
    static constexpr uint32_t COLORS_NUM = 1 << COLOR_BITS;
    static constexpr uint32_t MARKER_SHIFT = COLOR_BITS;
    static constexpr uint32_t COLOR_MASK = (1 << MARKER_SHIFT) - 1;
    static constexpr uint32_t MAX_MARKER_INDEX = (1 << (MARKER_INDEX_BITS - COLOR_BITS)) - 1;
    static constexpr uint32_t UNMARKER = 0;

public:
    NO_COPY_SEMANTIC(MarkerManager);
    NO_MOVE_SEMANTIC(MarkerManager);

    MarkerManager() = default;
    ~MarkerManager() = default;

    // Create new marker.
    // Only 4 markers are available at one moment.
    Marker CreateNewMarker()
    {
        ++markerIndex_;
        assert(markerIndex_ < MAX_MARKER_INDEX);

        for (size_t idx = 0; idx < COLORS_NUM; ++idx) {
            if (!colors_[idx]) {
                Marker marker = (markerIndex_ << COLOR_BITS) | idx;
                colors_[idx] = 1;
                return marker;
            }
        }

        std::cerr << "No free markers are available" << std::endl;
        UNREACHABLE();
    }

    void EraseMarker(Marker marker)
    {
        colors_[marker & COLOR_MASK] = 0;
    }

private:
    size_t markerIndex_ {0};
    std::bitset<COLORS_NUM> colors_;
};

}  // namespace compiler

#endif  // IR_MARKER_H
