#pragma once

#include <string.h>
#include <tas5805m.hpp>

#include "command.hpp"
#include "argtable3/argtable3.h"

extern tas5805m Tas5805m;

class ModulationCommand : public Command
{
    static inline const char *TAG = "CMD.MOD";

private:
    // enum for modulation modes
    typedef enum
    {
        MOD_BD,
        MOD_1SPW,
        MOD_HYBRID,
        MOD_INVALID
    } mod_mode_t;

    static mod_mode_t get_mod_mode(const char *arg)
    {
        if (strcmp(arg, "bd") == 0)
            return MOD_BD;
        if (strcmp(arg, "1spw") == 0)
            return MOD_1SPW;
        if (strcmp(arg, "hybrid") == 0)
            return MOD_HYBRID;
        return MOD_INVALID;
    }

    static TAS5805M_MOD_MODE map_mod_mode(mod_mode_t mode)
    {
        switch (mode)
        {
        case MOD_1SPW:
            return MOD_MODE_1SPW;
        case MOD_HYBRID:
            return MOD_MODE_HYBRID;
        default:
            return MOD_MODE_BD;
        }
    }

    static char *mod_mode_to_string(TAS5805M_MOD_MODE mode)
    {
        switch (mode)
        {
        case MOD_MODE_1SPW:
            return "1SPW";
        case MOD_MODE_HYBRID:
            return "HYBRID";
        case MOD_MODE_BD:
            return "BD";
        default:
            return "UNKNOWN";
        }
    }

    // enum for switching frequencies
    typedef enum
    {
        SW_768K,
        SW_384K,
        SW_480K,
        SW_576K,
        SW_INVALID
    } sw_freq_t;

    static sw_freq_t get_sw_freq(const char *arg)
    {
        if (strcmp(arg, "768k") == 0)
            return SW_768K;
        if (strcmp(arg, "384k") == 0)
            return SW_384K;
        if (strcmp(arg, "480k") == 0)
            return SW_480K;
        if (strcmp(arg, "576k") == 0)
            return SW_576K;
        return SW_INVALID;
    }

    static TAS5805M_SW_FREQ map_sw_freq(sw_freq_t freq)
    {
        switch (freq)
        {
        case SW_384K:
            return SW_FREQ_384K;
        case SW_480K:
            return SW_FREQ_480K;
        case SW_576K:
            return SW_FREQ_576K;
        default:
            return SW_FREQ_768K;
        }
    }

    static char *sw_freq_to_string(TAS5805M_SW_FREQ freq)
    {
        switch (freq)
        {
        case SW_FREQ_384K:
            return "384k";
        case SW_FREQ_480K:
            return "480k";
        case SW_FREQ_576K:
            return "576k";
        case SW_FREQ_768K:
            return "768k";

        default:
            return "UNKNOWN";
        }
    }

    typedef enum
    {
        BD_FREQ_80K,
        BD_FREQ_100K,
        BD_FREQ_120K,
        BD_FREQ_175K,
        BD_FREQ_INVALID
    } bd_freq_t;

    static bd_freq_t get_bd_freq(const char *arg)
    {
        if (strcmp(arg, "80k") == 0)
            return BD_FREQ_80K;
        if (strcmp(arg, "100k") == 0)
            return BD_FREQ_100K;
        if (strcmp(arg, "120k") == 0)
            return BD_FREQ_120K;
        if (strcmp(arg, "175k") == 0)
            return BD_FREQ_175K;
        return BD_FREQ_INVALID;
    }

    static TAS5805M_BD_FREQ map_bd_freq(bd_freq_t freq)
    {
        switch (freq)
        {
        case BD_FREQ_100K:
            return SW_FREQ_100K;
        case BD_FREQ_120K:
            return SW_FREQ_120K;
        case BD_FREQ_175K:
            return SW_FREQ_175K;
        default:
            return SW_FREQ_80K;
        }
    }

    static char *bd_freq_to_string(TAS5805M_BD_FREQ freq)
    {
        switch (freq)
        {
        case SW_FREQ_100K:
            return "100k";
        case SW_FREQ_120K:
            return "120k";
        case SW_FREQ_175K:
            return "175k";
        case SW_FREQ_80K:
            return "80k";
        default:
            return "UNKNOWN";
        }
    }

    // Handler function for the "mod" command
    static int mod_command_handler(int argc, char **argv)
    {
        int nerrors = arg_parse(argc, argv, (void **)&mod_args);
        if (nerrors > 0)
        {
            arg_print_errors(stderr, mod_args.end, "mod");
            ESP_LOGE("CMD", "Invalid command usage");
            return 1;
        }

        if (mod_args.mode->count == 0)
        {
            TAS5805M_MOD_MODE mode;
            TAS5805M_SW_FREQ freq;
            TAS5805M_BD_FREQ bd_freq;
            Tas5805m.getModulationMode(&mode, &freq, &bd_freq);
            ESP_LOGI(TAG, "Current modulation mode is %s, dsp processing freq: %s, output switching freq: %s", 
                mod_mode_to_string(mode), sw_freq_to_string(freq), bd_freq_to_string(bd_freq));
            return 0;
        }

        mod_mode_t _mode = get_mod_mode(mod_args.mode->sval[0]);
        if (_mode == MOD_INVALID)
        {
            ESP_LOGE(TAG, "%s: Invalid mode %s", __func__, mod_args.mode->sval[0]);
            return ESP_ERR_INVALID_ARG;
        }

        sw_freq_t _freq = get_sw_freq(mod_args.freq->sval[0]);
        if (_freq == SW_INVALID)
        {
            ESP_LOGE(TAG, "%s: Invalid frequency %s", __func__, mod_args.freq->sval[0]);
            return ESP_ERR_INVALID_ARG;
        }

        bd_freq_t _bd_freq = get_bd_freq(mod_args.bd_freq->sval[0]);
        if (_bd_freq == BD_FREQ_INVALID)
        {
            ESP_LOGE(TAG, "%s: Invalid bd frequency %s", __func__, mod_args.bd_freq->sval[0]);
            return ESP_ERR_INVALID_ARG;
        }

        TAS5805M_MOD_MODE mode = map_mod_mode(_mode);
        TAS5805M_SW_FREQ freq = map_sw_freq(_freq);
        TAS5805M_BD_FREQ bd_freq = map_bd_freq(_bd_freq);
        ESP_LOGI("CMD", "Setting modulation mode to %s, dsp processing freq: %s, switching freq: %s", 
            mod_mode_to_string(mode), sw_freq_to_string(freq), bd_freq_to_string(bd_freq));
        Tas5805m.setModulationMode(mode, freq, bd_freq);
        return 0;
    }

public:
    struct ModulationArgs
    {
        struct arg_str *mode;
        struct arg_str *freq;
        struct arg_str *bd_freq;
        struct arg_end *end;
    } args;

    static inline ModulationArgs mod_args = {
        arg_str0(NULL, NULL, "[mode]", "Modulation mode: bd, 1spw, hybrid"),
        arg_str0(NULL, NULL, "[freq]", "Frequency: 768k, 384k, 480k, 576k"),
        arg_str0(NULL, NULL, "[bd_freq]", "BD Frequency: 80k, 100k, 120k, 175k"),
        arg_end(3)};

    String getName()
    {
        return "mod";
    };

    esp_console_cmd_t getCommand()
    {
        return {
            .command = "mod",
            .help = "Control the modulation mode",
            .hint = NULL,
            .func = &mod_command_handler,
            .argtable = &mod_args};
    }
};