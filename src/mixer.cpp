#include "commands/mixer.hpp"

MixerCommand::MixerArgs MixerCommand::mixer_args = {
    arg_str0(NULL, NULL, "[ll|lr|rl|rr]", "Mixer channel, one of the listed: left to left, left to right, right to left, right to right"),
    arg_dbl0(NULL, NULL, "[gain]", "Mixer gain in decimal, 1.0 is 0dB, 0.5 is -6dB, 2.0 is +6dB"),
    arg_end(2)
};