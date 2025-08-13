#include "commands/volume.hpp"

VolumeCommand::VolumeArgs VolumeCommand::volume_args = {
    arg_int0(NULL, NULL, "<volume>", "Volume level (0..255)"),
    arg_end(1)
};