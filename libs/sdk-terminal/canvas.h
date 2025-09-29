#pragma once

#include <sdk-cli/style.h>
#include <sdk-io/text.h>
#include <sdk-meta/types.h>

namespace Sdk::Cli {

struct Canvas {
    u32   width, height;
    Style style;

    virtual ~Canvas() = default;

    virtual void draw(u32 x, u32 y, char c, Style style) = 0;

    virtual void clear() = 0;
};

struct CanvasTextEncoder : public Io::TextEncoderBase<> {
    Canvas& canvas;
    u32     x { 0 }, y { 0 };
    bool    autoNewline { true };
    Style   style;

    CanvasTextEncoder(Canvas& canvas, u32 x = 0, u32 y = 0,
                      Style style = Style::Default)
        : canvas(canvas),
          x(x),
          y(y) { }

    Res<usize> write(Bytes bytes) override {
        usize written = 0;
        for (auto b : bytes) {
            canvas.draw(x, y, (char) b, canvas.style);
            x++;
            written++;
            if (x >= canvas.width and autoNewline) {
                x = 0;
                y++;
            } else
                break;
        }
        return Ok(written);
    }

    Res<> write(byte b) override {
        canvas.draw(x, y, (char) b, canvas.style);
        x++;
        if (x >= canvas.width and autoNewline) {
            x = 0;
            y++;
        }
        return Ok();
    }
};

} // namespace Sdk::Cli
