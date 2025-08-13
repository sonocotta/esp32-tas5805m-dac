#pragma once

#include "esp_console.h"

class Command
{
public:
    virtual esp_console_cmd_t getCommand() = 0;
}; 
