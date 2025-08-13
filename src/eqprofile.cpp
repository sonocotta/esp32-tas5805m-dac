#include "commands/eqprofile.hpp"

EqProfileCommand::EqProfileArgs EqProfileCommand::eq_args = {
    arg_str0(NULL, NULL, "<l|r>", "Channel: left or right. If not in BIAMP mode, left applies to both channels"),
    arg_int0(NULL, NULL, "[profile]", "Profile number (0..19), 0 is flat, 1 is LF 60Hz, 2 is LF 70Hz, .. 10 is LF 150Hz, 11 is HF 60Hz, .. 20 is HF 150Hz"),
    arg_end(2)
};