#pragma once

// Represents a fragment generated during rasterization.
// Contains pixel coordinates, depth value, and interpolated varyings.
#include "data/Varyings.h"

namespace hw1 {

struct Fragment {
    int x;
    int y;
    float depth;

    Varyings varyings;
};

} // namespace hw1
