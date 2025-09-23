#pragma once

#include <sdk-math/funcs.h>
#include <sdk-meta/types.h>

namespace Ui {

using namespace Sdk;

enum struct GridUnitType {
    Auto = 0,
    PIXEL,
    Star
};

struct GridLength {
    f64          _unitValue;
    GridUnitType _unitType;

    constexpr GridLength(f64 value, GridUnitType unitType = GridUnitType::PIXEL)
        : _unitValue(value),
          _unitType(unitType) {
        if (Math::isNan(_unitValue) or Math::isInf(_unitValue)) {
            // TODO: throw exception
        }
        if ((_unitType != GridUnitType::PIXEL)
            && (_unitType != GridUnitType::Star)
            && (_unitType != GridUnitType::Auto)) {
            // TODO: throw exception
        }
    }

    inline bool isAbsolute() const { return _unitType == GridUnitType::PIXEL; }

    inline bool isStar() const { return _unitType == GridUnitType::Star; }

    inline bool isAuto() const { return _unitType == GridUnitType::Auto; }

    static GridLength const Auto;
};

inline constexpr GridLength GridLength::Auto { 0.0, GridUnitType::Auto };

} // namespace Ui
