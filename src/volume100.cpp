#include "commands/volume100.hpp"

Volume100Command::VolumeArgs Volume100Command::volume_args = {
    arg_int0(NULL, NULL, "<vol>", "Volume level (0..100)"),
    arg_end(1)
};