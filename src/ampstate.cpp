#include "commands/ampstate.hpp"

AmpStateCommand::AmpStateArgs AmpStateCommand::amp_args = {
    arg_str0(NULL, NULL, "<mode>", "Amp mode: play, mute, highz, sleep, deepsleep"),
    arg_end(1)
};