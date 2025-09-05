#include "commands/clipper.h"

ClipperCommand::ClipperArgs ClipperCommand::clipper_args = {
    arg_dbl0("g", "gain", "[gain]", "Clipper gain in dB, typically negative, e.g. -3.0"),
    arg_dbl0("l", "makeup_left", "[makeup_left]", "Left channel makeup gain in dB, typically positive, e.g. 3.0"),
    arg_dbl0("r", "makeup_right", "[makeup_right]", "Right channel makeup gain in dB, typically positive, e.g. 3.0"),
    arg_end(3)
};