#include "commands/dacmode.hpp"

DacModeCommand::DacModeArgs DacModeCommand::dacmode_args = {
    arg_str0(NULL, NULL, "<mode>", "DAC mode: btl, pbtl"),
    arg_end(1)
};