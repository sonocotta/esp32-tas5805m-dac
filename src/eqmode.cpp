#include "commands/eqmode.hpp"

EqModeCommand::EqModeArgs EqModeCommand::eq_args = {
    arg_int0(NULL, NULL, "[mode]", "EQ mode: 0 - ON, 1 - OFF, 2 - BIAMP, 3 - BIAMP_OFF"),
    arg_end(1)
};