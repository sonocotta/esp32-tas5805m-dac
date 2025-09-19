#include "commands/levelmeter.hpp"

TaskHandle_t LevelMeterCommand::taskHandle = nullptr;

LevelMeterCommand::LevelMeterArgs LevelMeterCommand::levelmeter_args = {
    arg_str0(NULL, NULL, "[action]", "Get level meter readings: on, off"),
    arg_end(1)
};