#include "commands/modmode.hpp"

ModulationCommand::ModulationArgs ModulationCommand::mod_args = {
    arg_str0(NULL, NULL, "[mode]", "Modulation mode: bd, 1spw, hybrid"),
    arg_str0(NULL, NULL, "[freq]", "Frequency: 768k, 384k, 480k, 576k"),
    arg_str0(NULL, NULL, "[bd_freq]", "BD Frequency: 80k, 100k, 120k, 175k"),
    arg_end(3)
};