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
            struct arg_dbl *gain;
            struct arg_dbl *makeup_left;
            struct arg_dbl *makeup_right;
            struct arg_end *end;
        } clipper_args;

        clipper_args.gain = arg_dbl0(NULL, NULL, "<gain>", "Clipper gain in dB (negative value)");
        clipper_args.makeup_left = arg_dbl0(NULL, NULL, "<makeup_left>", "Left channel makeup gain in dB (positive value)");
        clipper_args.makeup_right = arg_dbl0(NULL, NULL, "<makeup_right>", "Right channel makeup gain in dB (positive value)");
        clipper_args.end = arg_end(1);  

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
            double gain, makeup_left, makeup_right;
            Tas5805m.getClipperGain(&gain, &makeup_left, &makeup_right);

            ESP_LOGI(TAG, "Clipper gain is %d dB", (int)lround(gain));
            ESP_LOGI(TAG, "Left channel makeup gain is %d dB", (int)lround(makeup_left));
            ESP_LOGI(TAG, "Right channel makeup gain is %d dB", (int)lround(makeup_right));

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

            float gain = clipper_args.gain->dval[0];
            float makeup_left = clipper_args.makeup_left->dval[0];
            float makeup_right = clipper_args.makeup_right->dval[0];

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
        struct arg_dbl *gain;
        struct arg_dbl *makeup_left;
        struct arg_dbl *makeup_right;
        struct arg_end *end;
    };  

    // Declaration only; definition moved to clipper.cpp
    static ClipperArgs clipper_args;

    esp_console_cmd_t getCommand() 
    {
        return {
            .command = "clip",
            .help = "Get or set the clipper gain and makeup gain",
            .hint = NULL,
            .func = &clipper_command_handler,
            .argtable = &clipper_args
        };
    }

};
