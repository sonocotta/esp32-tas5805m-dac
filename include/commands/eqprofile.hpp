#pragma once

#include <string.h>
#include <tas5805m.hpp>

#include "command.hpp"
#include "argtable3/argtable3.h"

extern tas5805m Tas5805m;

class EqProfileCommand : public Command
{
    static constexpr const char *TAG = "CMD.EQ.PROFILE";

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

        struct
        {
            struct arg_str *channel;
            struct arg_int *profile;
            struct arg_end *end;
        } eq_args;

        eq_args.channel = arg_str0(NULL, NULL, "<channel>", "Channel: l=left, r=right");
        eq_args.profile = arg_int0(NULL, NULL, "<profile>", "EQ profile: 0=FLAT, 1=LF_60HZ, 2=LF_70HZ, 3=LF_80HZ, 4=LF_90HZ, 5=LF_100HZ, 6=LF_110HZ, 7=LF_120HZ, 8=LF_130HZ, 9=LF_140HZ, 10=LF_150HZ, 11=HF_60HZ, 12=HF_70HZ, 13=HF_80HZ, 14=HF_90HZ, 15=HF_100HZ, 16=HF_110HZ, 17=HF_120HZ, 18=HF_130HZ, 19=HF_140HZ, 20=HF_150HZ");      
        eq_args.end = arg_end(1);   

        int nerrors = arg_parse(argc, argv, (void **)&eq_args);
        if (nerrors != 0)
        {
            arg_print_errors(stderr, eq_args.end, argv[0]);
            return 1;
        }

        if (eq_args.channel->count == 0)
        {
            TAS5805M_EQ_PROFILE eq_profile_l, eq_profile_r;
            Tas5805m.getEqProfile(TAS5805M_EQ_CHANNELS_LEFT, &eq_profile_l);
            Tas5805m.getEqProfile(TAS5805M_EQ_CHANNELS_RIGHT, &eq_profile_r);
            ESP_LOGI(TAG, "EQ profile num is %d (left), which is %s",
                    eq_profile_l, tas5805m_eq_profile_names[eq_profile_l]);
            ESP_LOGI(TAG, "EQ profile num is %d (right), which is %s",
                    eq_profile_r, tas5805m_eq_profile_names[eq_profile_r]);
            return 0;
        }

        TAS5805M_EQ_CHANNELS channel = TAS5805M_EQ_CHANNELS_LEFT;
        if (strcmp(eq_args.channel->sval[0], "r") == 0)
        {
            channel = TAS5805M_EQ_CHANNELS_RIGHT;
        }
        else if (strcmp(eq_args.channel->sval[0], "l") != 0)
        {
            ESP_LOGE(TAG, "Invalid channel '%s', must be 'l' or 'r'", eq_args.channel->sval[0]);
            return 1;
        }

        if (eq_args.profile->count == 0)
        {
            TAS5805M_EQ_PROFILE eq_profile;
            Tas5805m.getEqProfile(channel, &eq_profile);
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

        ESP_LOGI(TAG, "Setting EQ profile (%d) #%d, which is %s", channel, profile,
                    tas5805m_eq_profile_names[profile]);
        Tas5805m.setEqProfile(channel, static_cast<TAS5805M_EQ_PROFILE>(profile));

        return 0;
    }

public:

    struct EqProfileArgs
    {
        struct arg_str *channel;
        struct arg_int *profile;
        struct arg_end *end;
    } args;

    // Declaration only; definition moved to eqprofile.cpp
    static EqProfileArgs eq_args;

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