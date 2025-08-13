#include "commands/gain.hpp"

GainCommand::GainArgs GainCommand::gain_args = {
    arg_int0(NULL, NULL, "<gain>", "Gain level, 0..31, 0 = 0dB, 31 = -15.5dB"),
    arg_end(1)
};