#include "commands/fault.hpp"

TaskHandle_t FaultCommand::taskHandle = nullptr;

FaultCommand::FaultArgs FaultCommand::fault_args = {
    arg_str0(NULL, NULL, "[action]", "Monitor fault state: on, off"),
    arg_end(1)
};