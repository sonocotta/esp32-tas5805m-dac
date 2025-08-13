#pragma once

#include <string.h>
#include <tas5805m.hpp>

#include "command.hpp"
#include "argtable3/argtable3.h"

extern tas5805m Tas5805m;

class EqModeCommand : public Command
{
    static constexpr const char *TAG = "CMD.EQ.MODE";

private:
    // Handler function for the "eqm" command
    static int eq_mode_command_handler(int argc, char **argv)
    {
        static constexpr const char *tas5805m_eq_mode_names[] = {
            "ON",
            "OFF",
            "BIAMP",
            "BIAMP_OFF",
        };

        int nerrors = arg_parse(argc, argv, (void **)&eq_args);
        if (nerrors != 0)
        {
            ESP_LOGE(TAG, "Error parsing arguments");
            return 1;
        }

        if (eq_args.mode->count == 0)
        {
            TAS5805M_EQ_MODE eq_mode;
            Tas5805m.getEqMode(&eq_mode);
            ESP_LOGI(TAG, "EQ mode is %d, which is %s", eq_mode,
                    tas5805m_eq_mode_names[eq_mode]);
            return 0;
        }

        int mode = eq_args.mode->ival[0];
        if (mode < 0 || mode >= 4)
        {
            ESP_LOGE(TAG, "Invalid mode %d, must be 0-3", mode);
            return 1;
        }
        
        ESP_LOGI(TAG, "Setting EQ mode #%d, which is %s", mode,
                    tas5805m_eq_mode_names[mode]);
        
                    TAS5805M_EQ_MODE _mode;
        switch (mode) {
        case 0:
            _mode = TAS5805M_EQ_MODE_ON;
            break;
        case 1:
            _mode = TAS5805M_EQ_MODE_OFF;
            break;
        case 2:
            _mode = TAS5805M_EQ_MODE_BIAMP;
            break;
        case 3:
            _mode = TAS5805M_EQ_MODE_BIAMP_OFF;
            break;
        default:
            ESP_LOGE(TAG, "Invalid mode %d, must be 0-3", mode);
            return 1;   

        }
        Tas5805m.setEqMode(_mode);

        return 0;
    }

public:

    struct EqModeArgs
    {
        struct arg_int *mode;
        struct arg_end *end;
    } args;

    static EqModeArgs eq_args;

    esp_console_cmd_t getCommand()
    {
        return {
            .command = "eqm",
            .help = "Control the EQ mode, OFF, ON (same EQ both channels) and BIAMP state (individual EQ for each channel)",
            .hint = NULL,
            .func = &eq_mode_command_handler,
            .argtable = &eq_args};
    }
};