#pragma once

#include <string.h>
#include <tas5805m.hpp>

#include "command.hpp"
#include "argtable3/argtable3.h"

extern tas5805m Tas5805m;

class EqCommand : public Command
{
    static constexpr const char *TAG = "CMD.EQ";

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

        if (eq_args.channel->count == 0)
        {
            ESP_LOGE(TAG, "Error: 'channel' argument is required.");
            return 1;
        }

        TAS5805M_EQ_CHANNELS channel = TAS5805M_EQ_CHANNELS_LEFT;
        if (strcmp(eq_args.channel->sval[0], "r") == 0)
        {
            channel = TAS5805M_EQ_CHANNELS_RIGHT;
        }
        else if (strcmp(eq_args.channel->sval[0], "l") != 0)
        {
            ESP_LOGE(TAG, "Invalid channel '%s', must be 'l' or 'r'", eq_args.channel->sval[0]);
            return 1;
        }

        if (eq_args.band->count == 0 || eq_args.gain->count == 0)
        {
            for (int band = 0; band < TAS5805M_EQ_BANDS; band++)
            {
                int gain;
                Tas5805m.getEqGain(channel, band, &gain);
                ESP_LOGI(TAG, "EQ band %d (%d Hz) has gain %d", band, tas5805m_eq_bands[band], gain);
            }
        }
        else
        {
            // Ensure band and gain are provided for "set"
            if (eq_args.band->count == 0 || eq_args.gain->count == 0)
            {
                ESP_LOGE(TAG, "Error: 'set' requires band and gain arguments.");
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

            ESP_LOGI(TAG, "Setting EQ band %d to gain %d", band, gain);
            Tas5805m.setEqGain(band, gain);
        }

        return 0;
    }

public:
    struct EqArgs
    {
        struct arg_str *channel;
        struct arg_int *band;
        struct arg_int *gain;
        struct arg_end *end;
    } args;

    static EqArgs eq_args;

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