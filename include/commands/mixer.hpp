#pragma once

#include <string.h>
#include <tas5805m.hpp>

#include "command.hpp"
#include "argtable3/argtable3.h"

extern tas5805m Tas5805m;

class MixerCommand: public Command
{
    static inline const char *TAG = "CMD.MIXER";

private:

    static enum MixerMode
    {
        STEREO,
        INV,
        MONO,
        LEFT,
        RIGHT,
        UNKNOWN
    } mixer_mode_t;

    static MixerMode get_mixer_mode(const char *arg)
    {
        if (strcmp(arg, "stereo") == 0)
            return STEREO;
        if (strcmp(arg, "inv") == 0)
            return INV;
        if (strcmp(arg, "mono") == 0)
            return MONO;
        if (strcmp(arg, "left") == 0)
            return LEFT;
        if (strcmp(arg, "right") == 0)
            return RIGHT;
        return UNKNOWN;
    }

    static const char *map_mixer_mode(TAS5805M_MIXER_MODE mode)
    {
        switch (mode)
        {
        case MIXER_STEREO:
            return "STEREO";
        case MIXER_STEREO_INVERSE:
            return "STEREO_INVERSE";
        case MIXER_MONO:
            return "MONO";
        case MIXER_LEFT:
            return "LEFT";
        case MIXER_RIGHT:
            return "RIGHT";
        default:
            return "UNKNOWN";
        }
    }

    static int mixer_command_handler(int argc, char **argv)
    {
        int nerrors = arg_parse(argc, argv, (void **)&mixer_args);
        if (nerrors > 0)
        {
            arg_print_errors(stderr, mixer_args.end, "mixer");
            ESP_LOGE("CMD", "Invalid command usage");
            return 1;
        }

        if (mixer_args.action->count == 0)
        {
            TAS5805M_MIXER_MODE mode;
            Tas5805m.getMixerMode(&mode);
            ESP_LOGI(TAG, "Current mixer mode is %s", map_mixer_mode(mode));
            return 0;
        }

        MixerMode mode = get_mixer_mode(mixer_args.action->sval[0]);

        switch (mode)
        {
            case STEREO:
                ESP_LOGI(TAG, "Mixer set to STEREO mode");
                ESP_ERROR_CHECK(Tas5805m.setMixerMode(MIXER_STEREO));
                break;
            case INV:
                ESP_LOGI(TAG, "Mixer set to STEREO_INVERSE mode");
                ESP_ERROR_CHECK(Tas5805m.setMixerMode(MIXER_STEREO_INVERSE));
                break;
            case MONO:
                ESP_LOGI(TAG, "Mixer set to MONO mode");
                ESP_ERROR_CHECK(Tas5805m.setMixerMode(MIXER_MONO));
                break;
            case LEFT:
                ESP_LOGI(TAG, "Mixer set to LEFT mode");
                ESP_ERROR_CHECK(Tas5805m.setMixerMode(MIXER_LEFT));
                break;
            case RIGHT:
                ESP_LOGI(TAG, "Mixer set to RIGHT mode");
                ESP_ERROR_CHECK(Tas5805m.setMixerMode(MIXER_RIGHT));
                break;

            default:
                ESP_LOGI(TAG, "Invalid mode! Use: stereo, inv, mono, left, right");
                return 1;
        }

        return 0;
    }

public:

    struct MixerArgs
    {
        struct arg_str *action;
        struct arg_end *end;
    };

    static inline MixerArgs mixer_args = {
        arg_str0(NULL, NULL, "[stereo|inv|mono|left|right]", "Mixer mode, one of the listed"),
        arg_end(1)
    };

    String getName()
    {
        return "mixer";
    };

    esp_console_cmd_t getCommand()
    {
        return {
            .command = "mixer",
            .help = "Control the mixer",
            .hint = NULL,
            .func = &mixer_command_handler,
            .argtable = &mixer_args
        };
    }
};