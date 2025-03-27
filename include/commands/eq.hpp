#pragma once

#include <string.h>
#include <tas5805m.hpp>

#include "command.hpp"
#include "argtable3/argtable3.h"

extern tas5805m Tas5805m;

class EqCommand : public Command
{
    static inline const char *TAG = "CMD.EQ";

private:
    // Handler function for the "eq" command
    static int eq_command_handler(int argc, char **argv)
    {
        int nerrors = arg_parse(argc, argv, (void **)&eq_args);
        if (nerrors != 0)
        {
            ESP_LOGE(TAG, "Error parsing arguments");
            return 1;
        }
        
        if (eq_args.action->count == 0)
        {
            bool eq_enabled;
            Tas5805m.getEqEnabled(&eq_enabled);
            ESP_LOGI(TAG, "EQ enable state is %d", eq_enabled);
            return 0;
        }
        
        const char *action = eq_args.action->sval[0]; // Get action argument
        
        if (strcmp(action, "on") == 0)
        {
            ESP_LOGI("CMD", "Enabling EQ");
            Tas5805m.setEqEnabled(true);
        }
        else if (strcmp(action, "off") == 0)
        {
            ESP_LOGI("CMD", "Disabling EQ");
            Tas5805m.setEqEnabled(false);
        }
        else if (strcmp(action, "get") == 0)
        {
            // Ensure band is provided for "get"
            if (eq_args.band->count == 0)
            {
                ESP_LOGE("CMD", "Error: 'get' requires band argument.");
                return 1;
            }

            int band = eq_args.band->ival[0];
            if (band < 0 || band >= TAS5805M_EQ_BANDS)
            {
                ESP_LOGE(TAG, "%s: Invalid band %d", __func__, band);
                return ESP_FAIL;
            }

            int gain;
            Tas5805m.getEqGain(band, &gain);

            ESP_LOGI("CMD", "EQ band %d (%d Hz) has gain %d", band, tas5805m_eq_bands[band], gain);
        }
        else if (strcmp(action, "set") == 0)
        {
            // Ensure band and gain are provided for "set"
            if (eq_args.band->count == 0 || eq_args.gain->count == 0)
            {
                ESP_LOGE("CMD", "Error: 'set' requires band and gain arguments.");
                return 1;
            }

            int band = eq_args.band->ival[0];
            if (band < 0 || band >= TAS5805M_EQ_BANDS)
            {
                ESP_LOGE(TAG, "%s: Invalid band %d", __func__, band);
                return ESP_FAIL;
            }

            int gain = eq_args.gain->ival[0];
            if (gain < TAS5805M_EQ_MIN_DB || gain > TAS5805M_EQ_MAX_DB)
            {
                ESP_LOGE(TAG, "%s: Invalid gain %d", __func__, gain);
                return ESP_ERR_INVALID_ARG;
            }

            ESP_LOGI("CMD", "Setting EQ band %d to gain %d", band, gain);
            Tas5805m.setEqGain(band, gain);
        }
        else
        {
            ESP_LOGE("CMD", "Invalid action: %s", action);
            return 1;
        }

        return 0;
    }

public:
    struct EqArgs
    {
        struct arg_str *action;
        struct arg_int *band;
        struct arg_int *gain;
        struct arg_end *end;
    } args;

    static inline EqArgs eq_args = {
        arg_str0(NULL, NULL, "[<on|off|set|get>]", "Action: switch on/off or retrieve/update"),
        arg_int0(NULL, NULL, "[band]", "Band number (0..14)"),
        arg_int0(NULL, NULL, "[gain]", "Gain level (-15..15 Db, default: 0)"),
        arg_end(3)};

    String getName()
    {
        return "eq";
    };

    esp_console_cmd_t getCommand()
    {
        return {
            .command = "eq",
            .help = "Control the equalizer",
            .hint = NULL,
            .func = &eq_command_handler,
            .argtable = &eq_args};
    }
};