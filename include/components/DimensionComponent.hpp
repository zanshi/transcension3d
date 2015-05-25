//
// Created by Benjamin Wiberg on 15-04-22.
//


#pragma once

namespace ex = entityx;

namespace sw {
    enum Dim {
        DIMENSION_ONE = 1 << 0,
        DIMENSION_TWO = 1 << 1,
        DIMENSION_BOTH = DIMENSION_ONE | DIMENSION_TWO
    };

    /**
     * DimensionComponent
     * Basic wrapper for a Dimensions enum value, representing which dimension(-s) the entity 'is in'.
     * Usage: the rendering process should only render entities located in the current
     * 'dimension', i.e. if current_dim == DIMENSION_ONE, render all entities in
     * DIMENSION_ONE or DIMENSION_BOTH.
     */
    struct DimensionComponent : public ex::Component<DimensionComponent> {
        Dim dimension_;
        DimensionComponent(Dim dim = Dim::DIMENSION_BOTH)
                : dimension_(dim) { };
    };
}