#include "commands/eq.hpp"

EqCommand::EqArgs EqCommand::eq_args = {
    arg_str1("c", "channel", "<l|r>", "Channel: left or right. If not in BIAMP mode, left applies to both channels"),
    arg_int0("b", "band", "[band]", "Band number (0..9)"),
    arg_int0("g", "gain", "[gain]", "Gain value (-155..0 dB)"),
    arg_end(3)
};