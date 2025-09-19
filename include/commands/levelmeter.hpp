#pragma once

#include <string.h>
#include <tas5805m.hpp>

#include "command.hpp"
#include "argtable3/argtable3.h"

#define MIN_DDB   -1200   // -120.0 dB
#define MAX_DDB   0       // 0.0 dB
#define METER_W   40      // width of each channel

static int prev_left_len  = -1;
static int prev_right_len = -1;

extern tas5805m Tas5805m;

class LevelMeterCommand : public Command
{
    static constexpr const char *TAG = "CMD.LVL";

private:

    // Handler function for the "levelmeter" command
    static int levelmeter_command_handler(int argc, char **argv)
    {
        int nerrors = arg_parse(argc, argv, (void **)&levelmeter_args);
        if (nerrors > 0)
        {
            arg_print_errors(stderr, levelmeter_args.end, "levelmeter");
            ESP_LOGE(TAG, "Invalid command usage");
            return 1;
        }

        if (levelmeter_args.action->count == 0)
        {
            float left_f, right_f;
            if (Tas5805m.getLevelMeter(&left_f, &right_f) != ESP_OK)
            {
                ESP_LOGE(TAG, "Failed to get level meter");
                return 1;
            }
            ESP_LOGI(TAG, "Level Meter (flt) - Left: %.6f, Right: %.6f", left_f, right_f);    

            int32_t left_db, right_db;
            if (Tas5805m.getLevelMeterDb(&left_db, &right_db) != ESP_OK)
            {
                ESP_LOGE(TAG, "Failed to get level meter in dB");
                return 1;
            }
            ESP_LOGI(TAG, "Level Meter (ddB) - Left: %d ddB, Right: %d ddB", left_db, right_db);

        }
        else
        {
            const char *action = levelmeter_args.action->sval[0];

            if (strcmp(action, "on") == 0)
            {
                if (taskHandle == NULL)
                {
                    ESP_LOGD(TAG, "Starting level meter task");
                    xTaskCreatePinnedToCore(
                        &taskLoop,   // Task function
                        "LvlMeterTask",  // Task name
                        4096,        // Stack size
                        NULL,        // Task parameters (pass the instance)
                        1,           // Priority (1 is normal)
                        &taskHandle, // Task handle
                        1            // Run on Core 1
                    );
                }
                else
                {
                    ESP_LOGI(TAG, "Level meter already enabled");
                }
            }
            else if (strcmp(action, "off") == 0)
            {
                if (taskHandle != NULL)
                {
                    ESP_LOGI(TAG, "Disabling level meter");
                    vTaskDelete(taskHandle);
                    taskHandle = NULL;
                }
                else
                {
                    ESP_LOGI(TAG, "Level meter already disabled");
                }
            }
            else
            {
                ESP_LOGI(TAG, "Invalid action! Use: on, off");
                return 1;
            }
        }
    
        return 0;
    }

public:
    static TaskHandle_t taskHandle;

    struct LevelMeterArgs
    {
        struct arg_str *action; 
        struct arg_end *end;
    };      

    // Declaration only; definition moved to levelmeter.cpp
    static LevelMeterArgs levelmeter_args;

    esp_console_cmd_t getCommand() {
       return {
            .command = "lvl",
            .help = "Get the current level meter readings from the TAS5805M",
            .hint = NULL,
            .func = &levelmeter_command_handler,
            .argtable = &levelmeter_args,
        };
    }

    static void taskLoop(void *pvParameters)
    {
        while (1)
        {
            LevelMeterCommand::readLevels();
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
    }
    
    static inline int level_to_len(int ddb) {
        if (ddb < MIN_DDB) ddb = MIN_DDB;
        if (ddb > MAX_DDB) ddb = MAX_DDB;
        return (ddb - MIN_DDB) * METER_W / (MAX_DDB - MIN_DDB);
    }

    static void draw_levels_inplace(int left_ddb, int right_ddb) {
        int left_len  = level_to_len(left_ddb);
        int right_len = level_to_len(right_ddb);

        // only update if something changed
        if (left_len == prev_left_len && right_len == prev_right_len)
            return;

        prev_left_len  = left_len;
        prev_right_len = right_len;

        // build left bar
        char left_buf[METER_W+1];
        for (int i = 0; i < left_len; i++)  left_buf[i] = '#';
        for (int i = left_len; i < METER_W; i++) left_buf[i] = ' ';
        left_buf[METER_W] = '\0';

        // build right bar
        char right_buf[METER_W+1];
        for (int i = 0; i < right_len; i++)  right_buf[i] = '#';
        for (int i = right_len; i < METER_W; i++) right_buf[i] = ' ';
        right_buf[METER_W] = '\0';

        // overwrite line in-place
        printf("\rL[%s]  R[%s]", left_buf, right_buf);
        fflush(stdout);
    }

    static void readLevels()
    {
        int32_t left_db, right_db;
        if (Tas5805m.getLevelMeterDb(&left_db, &right_db) != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to get level meter in dB");
            return;
        }
        draw_levels_inplace(left_db, right_db);
    }

};
