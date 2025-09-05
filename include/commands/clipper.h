#pragma once

#include <string.h>
#include <tas5805m.hpp>

#include "command.hpp"
#include "argtable3/argtable3.h"

extern tas5805m Tas5805m;

class ClipperCommand : public Command
{
    static constexpr const char *TAG = "CMD.CLIPPER";
private:
    // Handler function for the "clipper" command
    static int clipper_command_handler(int argc, char **argv)
    {
        struct {
            struct arg_int *gain;
            struct arg_int *makeup_left;
            struct arg_int *makeup_right;
            struct arg_end *end;
        } clipper_args;

        clipper_args.gain = arg_int0(NULL, NULL, "[gain]", "Clipper gain in deci-dB, typically negative, e.g. -30 for -3.0dB");
        clipper_args.makeup_left = arg_int0(NULL, NULL, "[makeup_left]", "Left channel makeup gain in dB, typically positive, e.g. 30 for 3.0dB");        
        clipper_args.makeup_right =  arg_int0(NULL, NULL, "[makeup_right]", "Right channel makeup gain in dB, typically positive, e.g. 30 for 3.0dB"),
        clipper_args.end = arg_end(3);  

        int nerrors = arg_parse(argc, argv, (void **)&clipper_args);
        if (nerrors != 0)
        {
            arg_print_errors(stderr, clipper_args.end, argv[0]);
            return 1;
        }

        if (clipper_args.gain->count == 0 &&
                clipper_args.makeup_left->count == 0 &&
                clipper_args.makeup_right->count == 0)
        {
            int32_t gain, makeup_left, makeup_right;
            Tas5805m.getClipperGain(&gain, &makeup_left, &makeup_right);

            ESP_LOGI(TAG, "Current clipper gain is %d deci-dB, left makeup is %d deci-dB, right makeup is %d deci-dB", 
                gain, makeup_left, makeup_right);
            return 0;
        }
        else
        {
            // Ensure all three parameters are provided for "set"
            if (clipper_args.gain->count == 0 || clipper_args.makeup_left->count == 0 || clipper_args.makeup_right->count == 0)
            {
                ESP_LOGE(TAG, "Error: 'set' requires gain, makeup_left and makeup_right arguments.");
                return 1;
            }

            int32_t gain = clipper_args.gain->ival[0];
            int32_t makeup_left = clipper_args.makeup_left->ival[0];
            int32_t makeup_right = clipper_args.makeup_right->ival[0];

            // Set the clipper gain and makeup gains
            esp_err_t err = Tas5805m.setClipperGain(gain, makeup_left, makeup_right);
            if (err != ESP_OK)
            {
                ESP_LOGE(TAG, "Failed to set clipper gain: %s", esp_err_to_name(err));
                return 1;
            }
        }

        return 0;
    }   

public:
    struct ClipperArgs
    {
        struct arg_int *gain;
        struct arg_int *makeup_left;
        struct arg_int *makeup_right;
        struct arg_end *end;
    };  

    // Declaration only; definition moved to clipper.cpp
    static ClipperArgs clipper_args;

    esp_console_cmd_t getCommand() 
    {
        return {
            .command = "clip",
            .help = "Get or set the soft-clipper gain and makeup gain. Use without arguments to get current values.",
            .hint = NULL,
            .func = &clipper_command_handler,
            .argtable = &clipper_args
        };
    }

};
