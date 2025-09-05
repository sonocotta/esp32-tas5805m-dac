#include "commands/clipper.h"

ClipperCommand::ClipperArgs ClipperCommand::clipper_args = {
    arg_int0(NULL, NULL, "[gain]", "Clipper gain in deci-dB, typically negative, e.g. -30 for -3.0dB"),
    arg_int0(NULL, NULL, "[makeup_left]", "Left channel makeup gain in dB, typically positive, e.g. 30 for 3.0dB"),
    arg_int0(NULL, NULL, "[makeup_right]", "Right channel makeup gain in dB, typically positive, e.g. 30 for 3.0dB"),
    arg_end(3)
};