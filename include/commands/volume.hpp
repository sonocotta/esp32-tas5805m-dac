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

    static float volume_to_db(uint8_t volume)
    {
        return -(volume - TAS5805M_VOLUME_DEFAULT) / 2.0;
    }

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
            uint8_t volume;
            Tas5805m.getVolume(&volume);
            ESP_LOGI(TAG, "Current volume is %d, which is %f Db", volume, volume_to_db(volume));
            return 0;
        }

        int volume = args.volume->ival[0];
        if (volume < TAS5805M_VOLUME_MIN || volume > TAS5805M_VOLUME_MAX)
        {
            ESP_LOGI(TAG, "Invalid volume level! Must be between %d and %d.", TAS5805M_VOLUME_MIN, TAS5805M_VOLUME_MAX);
            return 1;
        }

        ESP_LOGI(TAG, "Setting volume to %d% [%d..%d], which is %f Db", volume, TAS5805M_VOLUME_MIN, TAS5805M_VOLUME_MAX, volume_to_db(volume));
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
        arg_int0(NULL, NULL, "[volume]", "Volume level [0..255], default 48 (+0 Db)"),
        arg_end(1)
    };

    String getName()
    {
        return "volume";
    };

    esp_console_cmd_t getCommand()
    {
        return {
            .command = "volume",
            .help = "Set the volume level of the amp. Usage: volume [volume], where volume is between 0 (+24 Db) and 255 (Mute), default 28 (+O db).",
            .hint = NULL,
            .func = volume_command_handler,
            .argtable = &volume_args
        };
    }
};