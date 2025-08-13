#pragma once

#include <string.h>
#include "tas5805m.hpp"
#include "tas5805m-math.h"

#include "command.hpp"
#include "argtable3/argtable3.h"

extern tas5805m Tas5805m;

class MixerCommand : public Command
{
    static constexpr const char *TAG = "CMD.MIXER";

private:

    static int mixer_command_handler(int argc, char **argv)
    {
        int nerrors = arg_parse(argc, argv, (void **)&mixer_args);
        if (nerrors > 0)
        {
            arg_print_errors(stderr, mixer_args.end, "mixer");
            ESP_LOGE(TAG, "Invalid command usage");
            return 1;
        }

        if (mixer_args.channel->count == 0)
        {
            uint32_t gain_9_23;
            for (uint8_t channel = 0; channel < 4; channel++)
            {
                TAS5805M_MIXER_CHANNELS mixer_channel = static_cast<TAS5805M_MIXER_CHANNELS>(channel);
                Tas5805m.getMixerGainRaw(mixer_channel, &gain_9_23);
                ESP_LOGI(TAG, "Mixer gain for channel %d is 0x%08x, which is decimal %.2f", 
                    channel, gain_9_23, tas5805m_q9_23_to_float(gain_9_23));
            }
            return 0;
        }

        TAS5805M_MIXER_CHANNELS channel;
        if (strcmp(mixer_args.channel->sval[0], "ll") == 0)
            channel = TAS5805M_MIXER_CHANNEL_LEFT_TO_LEFT;
        else if (strcmp(mixer_args.channel->sval[0], "lr") == 0)
            channel = TAS5805M_MIXER_CHANNEL_LEFT_TO_RIGHT;
        else if (strcmp(mixer_args.channel->sval[0], "rl") == 0)
            channel = TAS5805M_MIXER_CHANNEL_RIGHT_TO_LEFT;
        else if (strcmp(mixer_args.channel->sval[0], "rr") == 0)
            channel = TAS5805M_MIXER_CHANNEL_RIGHT_TO_RIGHT;
        else
        {
            ESP_LOGE(TAG, "Invalid channel '%s'", mixer_args.channel->sval[0]);
            return 1;
        }

        if (mixer_args.gain->count == 0)
        {
            uint32_t gain_9_23;
            Tas5805m.getMixerGainRaw(channel, &gain_9_23);
            ESP_LOGI(TAG, "Mixer gain for channel %s is 0x%x, which is decimal %.2f", 
                mixer_args.channel->sval[0], gain_9_23, tas5805m_q9_23_to_float(gain_9_23));
            return 0;
        }

        float gain = mixer_args.gain->dval[0];

        ESP_LOGI(TAG, "Setting mixer gain for channel %s to %.2f (0x%08x)", 
            mixer_args.channel->sval[0], gain, tas5805m_float_to_q9_23(gain));
        esp_err_t err = Tas5805m.setMixerGain(channel, gain);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to set mixer gain: %s", esp_err_to_name(err));
            return 1;
        }

        return 0;
    }

public:
    struct MixerArgs
    {
        struct arg_str *channel;
        struct arg_dbl *gain;
        struct arg_end *end;
    };

    // Declaration only; definition moved to mixer.cpp
    static MixerArgs mixer_args;

    esp_console_cmd_t getCommand()
    {
        return {
            .command = "mixer",
            .help = "Control the mixer, set gain for a specified channel",
            .hint = NULL,
            .func = &mixer_command_handler,
            .argtable = &mixer_args};
    }
};