#pragma once

#include <string.h>
#include <tas5805m.hpp>

#include "command.hpp"
#include "argtable3/argtable3.h"

extern tas5805m Tas5805m;

class DacModeCommand : public Command
{
private:
    static inline const char *TAG = "CMD.DACMODE";

    // Enum for DAC modes
    typedef enum
    {
        DAC_MODE_BTL,
        DAC_MODE_PBTL,
        DAC_MODE_INVALID
    } dac_mode_t;

    // Function to parse DAC mode from string
    static dac_mode_t get_dac_mode(const char *arg)
    {
        if (strcmp(arg, "btl") == 0)
            return DAC_MODE_BTL;
        if (strcmp(arg, "pbtl") == 0)
            return DAC_MODE_PBTL;
        return DAC_MODE_INVALID;
    }

    // Handler function for the "dacmode" command
    static int dacmode_command_handler(int argc, char **argv)
    {
        struct
        {
            struct arg_str *mode;
            struct arg_end *end;
        } args;

        args.mode = arg_str0(NULL, NULL, "<mode>", "DAC mode: btl, pbtl");
        args.end = arg_end(1);

        int nerrors = arg_parse(argc, argv, (void **)&args);
        if (nerrors != 0)
        {
            arg_print_errors(stderr, args.end, argv[0]);
            return 1;
        }

        if (args.mode->count == 0)
        {
            TAS5805M_DAC_MODE mode;
            Tas5805m.getDacMode(&mode);
            ESP_LOGI(TAG, "Current DAC mode is %s", mode == TAS5805M_DAC_MODE_BTL ? "BTL" : "PBTL");
            return 0;
        }
        
        dac_mode_t mode = get_dac_mode(args.mode->sval[0]);

        switch (mode)
        {
        case DAC_MODE_BTL:
            ESP_LOGI(TAG, "Setting DAC mode to BTL");
            Tas5805m.setDacMode(TAS5805M_DAC_MODE_BTL);
            break;
        case DAC_MODE_PBTL:
            ESP_LOGI(TAG, "Setting DAC mode to PBTL");
            Tas5805m.setDacMode(TAS5805M_DAC_MODE_PBTL);
            break;
        case DAC_MODE_INVALID:
            ESP_LOGI(TAG, "Invalid DAC mode! Must be 'btl' or 'pbtl'.");
            return 1;
        }

        return 0;
    }

public:
    struct DacModeArgs
    {
        struct arg_str *mode;
        struct arg_end *end;
    };

    static inline DacModeArgs dacmode_args = {
        arg_str0(NULL, NULL, "[mode]", "DAC mode: btl, pbtl"),
        arg_end(1)};

    String getName()
    {
        return "dacmode";
    }

    esp_console_cmd_t getCommand()
    {
        return {
            .command = "dacmode",
            .help = "Set the DAC mode of the TAS5805M amplifier",
            .hint = NULL,
            .func = &dacmode_command_handler,
            .argtable = &dacmode_args};
    }
};