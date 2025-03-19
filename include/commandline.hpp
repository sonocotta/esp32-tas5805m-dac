#pragma once

#include "esp_console.h"
#include "argtable3/argtable3.h"
#include "linenoise/linenoise.h"
#include "esp_vfs_dev.h"

#include "commands/command.hpp"
#include "commands/tone.hpp"
#ifdef CONFIG_DAC_TAS5805M
#include "commands/ampstate.hpp"
#include "commands/volume.hpp"
#include "commands/dacmode.hpp"
#include "commands/eq.hpp"
#include "commands/modmode.hpp"
#include "commands/gain.hpp"
#include "commands/mixer.hpp" 
#include "commands/fault.hpp"
#endif

class CommandLine
{
private:

#ifdef CONFIG_DAC_TAS5805M
    const static int commands_size = 9;
#else
    const static int commands_size = 1;
#endif

    Command *commands[commands_size] = {
        new ToneCommand(),
        #ifdef CONFIG_DAC_TAS5805M
        new AmpStateCommand(),
        new VolumeCommand(),
        new DacModeCommand(),
        new EqCommand(),
        new ModulationCommand(),
        new GainCommand(),
        new MixerCommand(),
        new FaultCommand(),
        #endif
    };

    static inline const char *TAG = "CLI";

    // Read and process commands from Serial
    void process_serial_commands()
    {
        int ret;
        char *line = linenoise("CLI> ");
        if (line)
        {
            if (strlen(line) > 0)
            {
                linenoiseHistoryAdd(line);
                esp_console_run(line, &ret);
                switch (ret)
                {
                case ESP_ERR_NOT_FOUND:
                    ESP_LOGI(TAG, "Command not found");
                    break;
                case ESP_ERR_INVALID_ARG:
                    ESP_LOGI(TAG, "Invalid argument");
                    break;
                case ESP_OK:
                    break;
                default:
                    // ESP_LOGI(TAG, "Internal error");
                    break;
                }
            }
            free(line);
        }
    }
    
    void register_commands()
    {
        ESP_ERROR_CHECK(esp_console_register_help_command());

        for (int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++)
        {
            Command *cmd = commands[i];
            esp_console_cmd_t cmd_struct = cmd->getCommand();
            ESP_LOGI(TAG, "Registering command: %s", cmd->getName());
            ESP_ERROR_CHECK(esp_console_cmd_register(&cmd_struct));
        }
    }

public:

    void init()
    {
        esp_vfs_dev_uart_use_driver(0);

        esp_console_config_t console_config = ESP_CONSOLE_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_console_init(&console_config));

        linenoiseSetMultiLine(1);
        linenoiseHistorySetMaxLen(32);

        register_commands();

        #ifdef CONFIG_DAC_TAS5805M
        Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
        Tas5805m.init();
        #endif


        ESP_LOGI(TAG, "ESP32 Command Line Interface Ready! Type 'help' for a list of commands.");
    }

    void loop()
    {
        process_serial_commands();
    };
};
