#pragma once

#include <string.h>

#include "esp_console.h"

class Command
{
public:
    virtual String getName() = 0;
    virtual esp_console_cmd_t getCommand() = 0;
}; 
