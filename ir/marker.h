#ifndef IR_MARKER_H
#define IR_MARKER_H

#include "utils/macros.h"

#include <cstdint>
#include <bitset>
#include <iostream>

namespace compiler {

using Marker = uint64_t;

class MarkerManager final {
public:
    static constexpr size_t MARKER_NUMBER = sizeof(Marker);

public:
    NO_COPY_SEMANTIC(MarkerManager);
    NO_MOVE_SEMANTIC(MarkerManager);

    MarkerManager() = default;
    ~MarkerManager() = default;

    Marker CreateNewMarker()
    {
        busyMarkers_[firstNotBusyIdx_] = true;
        Marker newMarker = 2 << firstNotBusyIdx_;

        firstNotBusyIdx_ = CalculateNextNotBusyIdx();
        return newMarker;
    }

    void DeleteMarker(Marker marker)
    {
        busyMarkers_ &= marker;
        firstNotBusyIdx_ = CalculateMarkerExponent(marker);
    }

    static Marker GetEmptyMarker()
    {
        return 0;
    }

private:
    static size_t CalculateMarkerExponent(Marker marker)
    {
        Marker temp = 1;
        for (size_t idx = 0;; ++idx) {
            temp = temp << idx;
            if (marker == temp) {
                return idx;
            }
        }
        UNREACHABLE();
    }

    size_t CalculateNextNotBusyIdx() const
    {
        for (size_t idx = firstNotBusyIdx_ + 1; idx < MARKER_NUMBER; ++idx) {
            if (!busyMarkers_[idx]) {
                return idx;
            }
        }
        std::cerr << "All markers are busy" << std::endl;
        UNREACHABLE();
    }

private:
    size_t firstNotBusyIdx_ {0};

    std::bitset<MARKER_NUMBER> busyMarkers_ {0};
};

} // namespace compiler

#endif // IR_MARKER_H
