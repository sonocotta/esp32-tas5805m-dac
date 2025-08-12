#pragma once

#include <string.h>
#include <tas5805m.hpp>

#include "command.hpp"
#include "argtable3/argtable3.h"

extern tas5805m Tas5805m;

class EqProfileCommand : public Command
{
    static inline const char *TAG = "CMD.EQ.PROFILE";

private:
    // Handler function for the "eqprofile" command
    static int eq_profile_command_handler(int argc, char **argv)
    {
        static constexpr const char *tas5805m_eq_profile_names[] = {
            "FLAT",
            "LF_60HZ",
            "LF_70HZ",
            "LF_80HZ",
            "LF_90HZ",
            "LF_100HZ",
            "LF_110HZ",
            "LF_120HZ",
            "LF_130HZ",
            "LF_140HZ",
            "LF_150HZ",
            "HF_60HZ",
            "HF_70HZ",
            "HF_80HZ",
            "HF_90HZ",
            "HF_100HZ",
            "HF_110HZ",
            "HF_120HZ",
            "HF_130HZ",
            "HF_140HZ",
            "HF_150HZ"
        };

        int nerrors = arg_parse(argc, argv, (void **)&eq_args);
        if (nerrors != 0)
        {
            ESP_LOGE(TAG, "Error parsing arguments");
            return 1;
        }

        if (eq_args.profile->count == 0)
        {
            TAS5805M_EQ_PROFILE eq_profile;
            Tas5805m.getEqProfile(&eq_profile);
            ESP_LOGI(TAG, "EQ profile num is %d, which is %s", eq_profile,
                    tas5805m_eq_profile_names[eq_profile]);
            return 0;
        }

        int profile = eq_args.profile->ival[0];
        if (profile < 0 || profile >= TAS5805M_EQ_PROFILES)
        {
            ESP_LOGE(TAG, "%s: Invalid profile %d", __func__, profile);
            return ESP_FAIL;
        }

        ESP_LOGI("CMD", "Setting EQ profile #%d, which is %s", profile,
                    tas5805m_eq_profile_names[profile]);
        Tas5805m.setEqProfile(static_cast<TAS5805M_EQ_PROFILE>(profile));

        return 0;
    }

public:

    struct EqProfileArgs
    {
        struct arg_int *profile;
        struct arg_end *end;
    } args;

    static inline EqProfileArgs eq_args = {
        arg_int0(NULL, NULL, "[profile]", "Profile number (0..19), 0 is flat, 1 is LF 60Hz, 2 is LF 70Hz, .. 10 is LF 150Hz, 11 is HF 60Hz, .. 20 is HF 150Hz"),
        arg_end(1)};

    String getName()
    {
        return "eqp";
    };

    esp_console_cmd_t getCommand()
    {
        return {
            .command = "eqp",
            .help = "Control the EQ profiles, Low-pass and High-pass filters within 60...150Hz range, 10Hz steps",
            .hint = NULL,
            .func = &eq_profile_command_handler,
            .argtable = &eq_args};
    }
};