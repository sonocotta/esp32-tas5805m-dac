#pragma once

#include <string.h>
#include <tas5805m.hpp>

#include "command.hpp"
#include "argtable3/argtable3.h"

extern tas5805m Tas5805m;

class VolumeCommand : public Command
{
private:
    static inline const char *TAG = "CMD.VOLUME";

    // Handler function for the "volume" command
    static int volume_command_handler(int argc, char **argv)
    {
        struct
        {
            struct arg_int *volume;
            struct arg_end *end;
        } args;

        args.volume = arg_int0(NULL, NULL, "<volume>", "Volume level");
        args.end = arg_end(1);

        int nerrors = arg_parse(argc, argv, (void **)&args);
        if (nerrors != 0)
        {
            arg_print_errors(stderr, args.end, argv[0]);
            return 1;
        }

        if (args.volume->count == 0)
        {
            int volume;
            Tas5805m.getVolume(&volume);
            ESP_LOGI(TAG, "Current volume is %d", volume);
            return 0;
        }

        int volume = args.volume->ival[0];
        if (volume < TAS5805M_VOLUME_MIN || volume > TAS5805M_VOLUME_MAX)
        {
            ESP_LOGI(TAG, "Invalid volume level! Must be between %d and %d.", TAS5805M_VOLUME_MIN, TAS5805M_VOLUME_MAX);
            return 1;
        }

        ESP_LOGI(TAG, "Setting volume to %d% [%d..%d]", volume, TAS5805M_VOLUME_MIN, TAS5805M_VOLUME_MAX);
        Tas5805m.setVolume(volume);
        return 0;
    }

public:
    struct VolumeArgs
    {
        struct arg_int *volume;
        struct arg_end *end;
    };

    static inline VolumeArgs volume_args = {
        arg_int0(NULL, NULL, "[volume]", "Volume level, default 110"),
        arg_end(1)
    };

    String getName()
    {
        return "amp";
    };

    esp_console_cmd_t getCommand()
    {
        return {
            .command = "volume",
            .help = "Set the volume level of the amp. Usage: volume [volume]",
            .hint = NULL,
            .func = volume_command_handler,
            .argtable = &volume_args
        };
    }
};