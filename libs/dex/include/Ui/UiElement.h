#pragma once

#include <Ui/UiTypes.h>

namespace Ui {

struct UiElement {
    virtual ~UiElement() = default;

    Size PreferredSize;
};

} // namespace Ui
