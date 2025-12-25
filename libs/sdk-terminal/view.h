#pragma once

#include <sdk-meta/iter.h>
#include <sdk-meta/list.h>
#include <sdk-meta/types.h>
#include <sdk-terminal/canvas.h>
#include <sdk-terminal/style.h>
#include <sdk-text/format.h>

namespace Sdk::Cli {

struct View {
    u32 width, height;

    virtual ~View() = default;

    virtual void draw(u32 x, u32 y, Canvas&) = 0;
};

struct Border : View {
    enum BorderType {
        Space,
        SingleLine,
        DoubleLine,
        Custom
    };
    static constexpr unsigned char BorderTypeCodes[][6] = {
        { ' ',  ' ',  ' ',  ' ',  ' ',  ' '  }, // Space
        { 0xda, 0xbf, 0xc0, 0xd9, 0xc4, 0xb3 }, // SingleLine
        { 0xc9, 0xbb, 0xc8, 0xbc, 0xcd, 0xba }, // DoubleLine
        { ' ',  ' ',  ' ',  ' ',  ' ',  ' '  }  // Custom
    };

    View* child { nullptr };
    Style borderStyle;
    struct {
        char tl, tr, bl, br;
        char h, v;
    } codes;

    inline void resize(u32 w, u32 h) {
        width  = w;
        height = h;
        if (child) {
            child->width  = width - 2;
            child->height = height - 2;
        }
    }

    inline void draw(u32 offsetX, u32 offsetY, Canvas& canvas) override {
        // clang-format off
        // Top and bottom borders
        for (u32 x = 0; x < width; x++) {
            canvas.draw(offsetX + x, offsetY, codes.h, borderStyle);
            canvas.draw(offsetX + x, offsetY + height - 1, codes.h, borderStyle);
        }

        // Left and right borders
        for (u32 y = 0; y < height; y++) {
            canvas.draw(offsetX, offsetY + y, codes.v, borderStyle);
            canvas.draw(offsetX + width - 1, offsetY + y, codes.v, borderStyle);
        }

        // Corners
        canvas.draw(offsetX, offsetY, codes.tl, borderStyle);
        canvas.draw(offsetX + width - 1, offsetY, codes.tr, borderStyle);
        canvas.draw(offsetX, offsetY + height - 1, codes.bl, borderStyle);
        canvas.draw(offsetX + width - 1, offsetY + height - 1, codes.br, borderStyle);

        if (child) {
            child->draw(offsetX + 1, offsetY + 1, canvas);
        }

        // clang-format on
    }

    inline void setBorderType(BorderType type) {
        if (type != Custom) {
            for (u32 i = 0; i < 6; i++) {
                ((char*) &codes)[i] = BorderTypeCodes[type][i];
            }
        }
    }
};

template <typename T>
    requires requires { typename Sdk::Text::Formatter<T>; }
struct ListView : View {
    List<T> items;
    Style   styleOnItem;
    Style   styleOnSelected;
    bool    index { false };
    bool    selectable { false };
    u32     selectedIndex { 0 };
    u32     startIndex { 0 };

    inline void draw(u32 offsetX, u32 offsetY, Canvas& canvas) override {
        Text::StringBuilder builder(width + 1);
        CanvasTextEncoder   encoder(canvas, offsetX, offsetY);
        encoder.autoNewline = false;

        auto writeStr = [&](usize line) {
            if (builder.len() > width) {
                builder.reduce(4);
                builder.append("..."s);
            };
            encoder.style = (selectable and line == selectedIndex)
                              ? styleOnSelected
                              : styleOnItem;
            try$(encoder.writeStr(builder.str()));
        };

        if (index) {
            u8 indexWidth = Text::numDigits(items.len());
            items.iter().index().skip(startIndex).limit(height).forEach$({
                Text::format(
                    builder,
                    "{}. {}",
                    Text::aligned(x.v0, Text::Align::RIGHT, indexWidth),
                    x.v1);
                writeStr(x.v0);
            });
        } else {
            items.iter().skip(startIndex).limit(height).forEach$({
                Text::format(builder, "{}", x);
                writeStr(x);
            });
        }
    }
};

struct ToggleView : View {
    Str   label;
    bool  selected { false };
    Style styleOnLabel;
    Style styleOnSelected;
};

} // namespace Sdk::Cli
