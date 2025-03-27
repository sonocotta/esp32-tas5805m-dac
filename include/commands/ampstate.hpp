#pragma once

#include <string.h>
#include <Wire.h>
#include <tas5805m.hpp>

#include "command.hpp"
#include "argtable3/argtable3.h"

extern tas5805m Tas5805m;

// Enum for AMP states
typedef enum {
    AMP_PLAY,
    AMP_HIGHZ,
    AMP_SLEEP,
    AMP_MUTE,
    AMP_DEEP_SLEEP,
    AMP_INVALID
} amp_state_t;


class AmpStateCommand : public Command
{


    static inline const char *TAG = "CMD.AMP";
    private:

    typedef enum {
        AMP_PLAY,
        AMP_HIGHZ,
        AMP_SLEEP,
        AMP_MUTE,
        AMP_DEEP_SLEEP,
        AMP_INVALID
    } amp_state_t;

    static amp_state_t get_amp_state(const char *arg) {
        if (strcmp(arg, "play") == 0) return AMP_PLAY;
        if (strcmp(arg, "highz") == 0) return AMP_HIGHZ;
        if (strcmp(arg, "sleep") == 0) return AMP_SLEEP;
        if (strcmp(arg, "mute") == 0) return AMP_MUTE;
        if (strcmp(arg, "deepsleep") == 0) return AMP_DEEP_SLEEP;
        return AMP_INVALID;
    }

    static char* map_amp_state(TAS5805M_CTRL_STATE state) 
    {
        switch (state)
        {
        case TAS5805M_CTRL_PLAY:
            return "PLAY";
        case TAS5805M_CTRL_HI_Z:
            return "HIGH-Z";
        case TAS5805M_CTRL_SLEEP:
            return "SLEEP";
        case TAS5805M_CTRL_MUTE:
            return "MUTE";
        case TAS5805M_CTRL_DEEP_SLEEP:
            return "DEEP SLEEP";
        default:
            return "UNKNOWN";
        }
    }

    // Handler function for the "amp" command
    static int amp_command_handler(int argc, char **argv)
    {
        int nerrors = arg_parse(argc, argv, (void **)&amp_args);
        if (nerrors > 0)
        {
            arg_print_errors(stderr, amp_args.end, "amp");
            ESP_LOGE("CMD", "Invalid command usage");
            return 1;
        }

        if (amp_args.mode->count == 0)
        {
            TAS5805M_CTRL_STATE state;
            Tas5805m.getState(&state);
            ESP_LOGI(TAG, "Current AMP state is %s", map_amp_state(state));
            return 0;
        }

        amp_state_t state = get_amp_state(amp_args.mode->sval[0]);

        switch (state)
        {
            case AMP_PLAY:
                ESP_LOGI(TAG, "AMP set to PLAY mode");
                ESP_ERROR_CHECK(Tas5805m.setState(TAS5805M_CTRL_PLAY));
                break;
            case AMP_HIGHZ:
                ESP_LOGI(TAG, "AMP set to HIGH-Z mode");
                ESP_ERROR_CHECK(Tas5805m.setState(TAS5805M_CTRL_HI_Z));
                break;
            case AMP_SLEEP:
                ESP_LOGI(TAG, "AMP set to SLEEP mode");
                ESP_ERROR_CHECK(Tas5805m.setState(TAS5805M_CTRL_SLEEP));
                break;
            case AMP_MUTE:
                ESP_LOGI(TAG, "AMP set to MUTE mode");
                ESP_ERROR_CHECK(Tas5805m.setState(TAS5805M_CTRL_MUTE));
                break;
            case AMP_DEEP_SLEEP:
                ESP_LOGI(TAG, "AMP set to DEEP SLEEP mode");
                ESP_ERROR_CHECK(Tas5805m.setState(TAS5805M_CTRL_DEEP_SLEEP));
                break;

            default:
                ESP_LOGI(TAG, "Invalid mode! Use: play, mute, highz, sleep, deepsleep");
                return 1;
        }

        return 0;
    }

public:

    struct AmpStateArgs
    {
        struct arg_str *mode;
        struct arg_end *end;
    };

    static inline AmpStateArgs amp_args = {
        arg_str0(NULL, NULL, "<mode>", "Amp mode: play, mute, highz, sleep, deepsleep"),
        arg_end(1)
    };

    String getName()
    {
        return "amp";
    }

    esp_console_cmd_t getCommand()
    {
        return {
            .command = "amp",
            .help = "Control the amplifier: play, mute, highz, sleep, deepsleep",
            .hint = NULL,
            .func = &amp_command_handler,
            .argtable = &amp_args
        };
    }
};
