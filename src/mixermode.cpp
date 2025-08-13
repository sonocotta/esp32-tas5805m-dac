#include "commands/mixermode.hpp"

MixerModeCommand::MixerModeArgs MixerModeCommand::mixer_mode_args = {
    arg_str0(NULL, NULL, "[stereo|inv|mono|left|right]", "Mixer mode, one of the listed"),
    arg_end(1)
};