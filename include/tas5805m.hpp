#pragma once

#include <inttypes.h>
#include <stddef.h>
#include <Wire.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_system.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include "esp_log.h"

#include "tas5805m.h"

// #include "startup/tas5805m.h"

// #define I2C_MASTER_NUM I2C_NUM_0   /*!< I2C port number for master dev */
// #define WRITE_BIT I2C_MASTER_WRITE /*!< I2C master write */
// #define READ_BIT I2C_MASTER_READ   /*!< I2C master read */
// #define ACK_CHECK_EN 0x1           /*!< I2C master will check ack from slave*/
// #define ACK_CHECK_DIS 0x0          /*!< I2C master will not check ack from slave */
// #define ACK_VAL I2C_MASTER_ACK     /*!< I2C ack value */
// #define NACK_VAL I2C_MASTER_NACK   /*!< I2C nack value */

// #define I2C_TAS5805M_MASTER_NUM I2C_MASTER_NUM /*!< I2C port number for master dev */

// #define TAS5805M_ADDRESS 0x2D /*!< 7-bit address with a 15k pull up resistor */
// #ifdef PIN_DAC_PWDN
// #define TAS5805M_GPIO_PDN PIN_DAC_PWDN
// #else
// #define TAS5805M_GPIO_PDN GPIO_NUM_5 /*!< GPIO used for !PDN pin */
// #endif
// #define TAS5805M_GPIO_PDN_MASK ((1ULL << TAS5805M_GPIO_PDN))

// #define TAS5805M_RESET_CTRL 0x01
// #define TAS5805M_DEVICE_CTRL_1 0x02
// #define TAS5805M_DEVICE_CTRL_2 0x03
// #define TAS5805M_I2C_PAGE_AUTO_INC 0x0f
// #define TAS5805M_SIG_CH_CTRL 0x28
// #define TAS5805M_CLOCK_DET_CTRL 0x29
// #define TAS5805M_SDOUT_SEL 0x30
// #define TAS5805M_I2S_CTRL 0x31
// #define TAS5805M_SAP_CTRL1 0x33
// #define TAS5805M_SAP_CTRL2 0x34
// #define TAS5805M_SAP_CTRL3 0x35
// #define TAS5805M_FS_MON 0x37
// #define TAS5805M_BCK_MON 0x38
// #define TAS5805M_CLKDET_STATUS 0x39
// #define TAS5805M_CHANNEL_FORCE_HIZ 0x40
// #define TAS5805M_DIG_VOL_CTRL 0x4c
// #define TAS5805M_DIG_VOL_CTRL2 0x4e
// #define TAS5805M_DIG_VOL_CTRL3 0x4f
// #define TAS5805M_AUTO_MUTE_CTRL 0x50
// #define TAS5805M_AUTO_MUTE_TIME 0x51
// #define TAS5805M_ANA_CTRL 0x53
// #define TAS5805M_AGAIN 0x54
// #define TAS5805M_BQ_WR_CTRL1 0x5c
// #define TAS5805M_DAC_CTRL 0x5d
// #define TAS5805M_ADR_PIN_CTRL 0x60
// #define TAS5805M_ADR_PIN_CONFIG 0x61
// #define TAS5805M_DSP_MISC 0x66
// #define TAS5805M_DIE_ID 0x67
// #define TAS5805M_POWER_STATE 0x68
// #define TAS5805M_AUTOMUTE_STATE 0x69
// #define TAS5805M_PHASE_CTRL 0x6a
// #define TAS5805M_SS_CTRL0 0x6b
// #define TAS5805M_SS_CTRL1 0x6c
// #define TAS5805M_SS_CTRL2 0x6d
// #define TAS5805M_SS_CTRL3 0x6e
// #define TAS5805M_SS_CTRL4 0x6f
// #define TAS5805M_CHAN_FAULT 0x70
// #define TAS5805M_GLOBAL_FAULT1 0x71
// #define TAS5805M_GLOBAL_FAULT2 0x72
// #define TAS5805M_OT_WARNING 0x73
// #define TAS5805M_PIN_CONTROL1 0x74
// #define TAS5805M_PIN_CONTROL2 0x75
// #define TAS5805M_MISC_CONTROL 0x76
// #define TAS5805M_FAULT_CLEAR 0x78

    class tas5805m
    {
    private:
        TwoWire *wire;

    // protected:
    //     esp_err_t _write_byte(uint8_t register_name, uint8_t value);
    //     esp_err_t _write_bytes(uint8_t *reg, int regLen, uint8_t *data, int datalen);
    //     esp_err_t _read(uint8_t register_name, uint8_t *data_rd, size_t size);
    //     esp_err_t _read_byte(uint8_t register_name, uint8_t *data_rd);
    //     esp_err_t _transmit_registers(const tas5805m_cfg_reg_t *conf_buf, int size);

    public:
        tas5805m(TwoWire *wire) : 
            wire(wire) {};

        esp_err_t deinit() {
            return tas5805m_deinit();
        }

        esp_err_t init() {
            return tas5805m_init();
        };

// esp_err_t getFaultState(uint8_t *, uint8_t *, uint8_t *) {
//     esp_err_t ret = ESP_OK;
//     i2c_cmd_handle_t cmd = i2c_cmd_link_create();
//     i2c_master_start(cmd);
//     i2c_master_write_byte(cmd, TAS5805M_ADDRESS << 1 | WRITE_BIT, ACK_CHECK_EN);
//     i2c_master_write_byte(cmd, TAS5805M_CHAN_FAULT, ACK_CHECK_EN);
//     i2c_master_stop(cmd);
//     ret = i2c_master_cmd_begin(I2C_TAS5805M_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
//     i2c_cmd_link_delete(cmd);
//     if (ret != ESP_OK)
//     {
//         ESP_LOGE(TAG, "I2C ERROR");
//     }

//     vTaskDelay(1 / portTICK_RATE_MS);
//     cmd = i2c_cmd_link_create();
//     i2c_master_start(cmd);
//     i2c_master_write_byte(cmd, TAS5805M_ADDRESS << 1 | READ_BIT, ACK_CHECK_EN);
//     i2c_master_read_byte(cmd, h70, ACK_VAL);
//     i2c_master_read_byte(cmd, h71, ACK_VAL);
//     i2c_master_read_byte(cmd, h72, NACK_VAL);
//     i2c_master_stop(cmd);
//     ret = i2c_master_cmd_begin(I2C_TAS5805M_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
//     i2c_cmd_link_delete(cmd);
//     return ret;
        // };

        // esp_err_t clearFaultState() {
//     return _write_byte(TAS5805M_FAULT_CLEAR, (uint8_t)0b10000000);
        // };

        // esp_err_t getGain(uint8_t *) {
//     uint8_t value = 0;
//     esp_err_t ret = _read_byte(TAS5805M_AGAIN, &value);
//     *volume = value << 3;
//     return ret;
        // };

        // esp_err_t setGain(uint8_t) {
//     uint8_t volume = value >> 3;
//     return _write_byte(TAS5805M_AGAIN, volume);
        // };

        esp_err_t getVolume(int *vol) {
            return tas5805m_get_volume(vol);
        };

        esp_err_t setVolume(int vol) {
            return tas5805m_set_volume(vol);
        };

        esp_err_t getState(TAS5805M_CTRL_STATE *state) {
            return tas5805m_get_state(state);
        };

        esp_err_t setState(TAS5805M_CTRL_STATE state) {
            return tas5805m_set_state(state);
        };

        esp_err_t getDacMode(TAS5805M_DAC_MODE *mode) {
            return tas5805m_get_dac_mode(mode);
        };

        esp_err_t setDacMode(TAS5805M_DAC_MODE mode) {
            return tas5805m_set_dac_mode(mode);
        };

        esp_err_t getEqEnabled(bool *enabled) {
            return tas5805m_get_eq(enabled);
        };

        esp_err_t setEqEnabled(bool enabled) {
            return tas5805m_set_eq(enabled);
        };

        esp_err_t getEqGain(int band, int *gain) {
            return tas5805m_get_eq_gain(band, gain);
        };

        esp_err_t setEqGain(int band, int gain) {
            return tas5805m_set_eq_gain(band, gain);
        };

        esp_err_t getModulationMode(TAS5805M_MOD_MODE *mode, TAS5805M_SW_FREQ *freq, TAS5805M_BD_FREQ *bd_freq) {
            return tas5805m_get_modulation_mode(mode, freq, bd_freq);
        };

        esp_err_t setModulationMode(TAS5805M_MOD_MODE mode, TAS5805M_SW_FREQ freq, TAS5805M_BD_FREQ bd_freq) {
            return tas5805m_set_modulation_mode(mode, freq, bd_freq);
        };

        esp_err_t getAnalogGain(int *gain) {
            return tas5805m_get_again(gain);
        };

        esp_err_t setAnalogGain(int gain) {
            return tas5805m_set_again(gain);
        };

        esp_err_t getMixerMode(TAS5805M_MIXER_MODE *mode) {
            return tas5805m_get_mixer_mode(mode);
        };

        esp_err_t setMixerMode(TAS5805M_MIXER_MODE mode) {
            return tas5805m_set_mixer_mode(mode);
        };

        esp_err_t getFsFreq(TAS5805M_FS_FREQ *freq) {
            return tas5805m_get_fs_freq(freq);
        };

        esp_err_t getBckRatio(uint8_t *ratio) {
            return tas5805m_get_bck_ratio(ratio);
        };

        esp_err_t getPowerState(TAS5805M_CTRL_STATE *state) {
            return tas5805m_get_power_state(state);
        };

        esp_err_t getAutomuteState(bool *is_r_muted, bool *is_l_muted) {
            return tas5805m_get_automute_state(is_r_muted, is_l_muted);
        };

        esp_err_t getFaultState(TAS5805M_FAULT* fault) {
            return tas5805m_get_faults(fault);
        };

        esp_err_t clearFaultState() {
            return tas5805m_clear_faults();
        };

        void decodeFaults(TAS5805M_FAULT fault){
            tas5805m_decode_faults(fault);
        }
    };


// esp_err_t tas5805m::init()
// {
// }

// esp_err_t tas5805m::begin()
// {
// }

// esp_err_t tas5805m::_transmit_registers(const tas5805m_cfg_reg_t *conf_buf, int size) {
//   int i = 0;
//   esp_err_t ret = ESP_OK;
//   while (i < size) {
//     switch (conf_buf[i].offset) {
//       case CFG_META_SWITCH:
//         // Used in legacy applications.  Ignored here.
//         break;
//       case CFG_META_DELAY:
//         vTaskDelay(conf_buf[i].value / portTICK_RATE_MS);
//         break;
//       case CFG_META_BURST:
//         ret = _write_bytes((unsigned char *)(&conf_buf[i + 1].offset), 1,
//                                    (unsigned char *)(&conf_buf[i + 1].value), conf_buf[i].value);
//         i += (conf_buf[i].value / 2) + 1;
//         break;
//       case CFG_END_1:
//         if (CFG_END_2 == conf_buf[i + 1].offset &&
//             CFG_END_3 == conf_buf[i + 2].offset) {
//           ESP_LOGI(TAG, "End of tas5805m reg: %d", i);
//         }
//         break;
//       default:
//         ret = _write_bytes((unsigned char *)(&conf_buf[i].offset), 1,
//                                    (unsigned char *)(&conf_buf[i].value), 1);
//         break;
//     }
//     i++;
//   }
//   if (ret != ESP_OK) {
//     ESP_LOGE(TAG, "Fail to load configuration to tas5805m");
//     return ESP_FAIL;
//   }
//   ESP_LOGI(TAG, "%s:  write %d registers successfully", __FUNCTION__, i);
//   return ret;
// }

// esp_err_t tas5805m::getFaultState(uint8_t *h70, uint8_t *h71, uint8_t *h72)
// {
// }

// esp_err_t tas5805m::clearFaultState() 
// {
// }

// esp_err_t tas5805m::_read(uint8_t register_name, uint8_t *data_rd, size_t size)
// {
//     if (size == 0)
//     {
//         return ESP_OK;
//     }
//     i2c_cmd_handle_t cmd = i2c_cmd_link_create();
//     i2c_master_start(cmd);
//     i2c_master_write_byte(cmd, (TAS5805M_ADDRESS << 1) | READ_BIT, ACK_CHECK_EN);
//     i2c_master_write_byte(cmd, register_name, ACK_CHECK_EN);
//     if (size > 1)
//     {
//         i2c_master_read(cmd, data_rd, size - 1, ACK_VAL);
//     }
//     i2c_master_read_byte(cmd, data_rd + size - 1, NACK_VAL);
//     i2c_master_stop(cmd);
//     esp_err_t ret = i2c_master_cmd_begin(I2C_TAS5805M_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
//     i2c_cmd_link_delete(cmd);
//     return ret;
// }

// esp_err_t tas5805m::_read_byte(uint8_t register_name, uint8_t *data)
// { 
//     esp_err_t ret = ESP_OK;
//     i2c_cmd_handle_t cmd = i2c_cmd_link_create();
//     i2c_master_start(cmd);
//     i2c_master_write_byte(cmd, TAS5805M_ADDRESS << 1 | WRITE_BIT, ACK_CHECK_EN);
//     i2c_master_write_byte(cmd, register_name, ACK_CHECK_EN);
//     i2c_master_stop(cmd);
//     ret = i2c_master_cmd_begin(I2C_TAS5805M_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
//     i2c_cmd_link_delete(cmd);
//     if (ret != ESP_OK)
//     {
//         ESP_LOGE(TAG, "I2C ERROR");
//     }
//     vTaskDelay(1 / portTICK_RATE_MS);
//     cmd = i2c_cmd_link_create();
//     i2c_master_start(cmd);
//     i2c_master_write_byte(cmd, TAS5805M_ADDRESS << 1 | READ_BIT, ACK_CHECK_EN);
//     i2c_master_read_byte(cmd, data, NACK_VAL);
//     i2c_master_stop(cmd);
//     ret = i2c_master_cmd_begin(I2C_TAS5805M_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
//     i2c_cmd_link_delete(cmd);
//     return ret;
// }

// esp_err_t tas5805m::_write_byte(uint8_t register_name, uint8_t value)
// {
//     esp_err_t ret;
//     i2c_cmd_handle_t cmd = i2c_cmd_link_create();
//     i2c_master_start(cmd);
//     i2c_master_write_byte(cmd, TAS5805M_ADDRESS << 1 | WRITE_BIT, ACK_CHECK_EN);
//     i2c_master_write_byte(cmd, register_name, ACK_CHECK_EN);
//     i2c_master_write_byte(cmd, value, ACK_CHECK_EN);
//     i2c_master_stop(cmd);
//     ret = i2c_master_cmd_begin(I2C_TAS5805M_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
//     i2c_cmd_link_delete(cmd);
//     return ret;
// }

// esp_err_t tas5805m::_write_bytes(uint8_t *reg, int regLen, uint8_t *data, int datalen)
// {
//   esp_err_t ret = ESP_OK;
//   i2c_cmd_handle_t cmd = i2c_cmd_link_create();
//   ret |= i2c_master_start(cmd);
//   ret |= i2c_master_write_byte(cmd, TAS5805M_ADDRESS << 1 | WRITE_BIT, ACK_CHECK_EN);
//   ret |= i2c_master_write(cmd, reg, regLen, ACK_CHECK_EN);
//   ret |= i2c_master_write(cmd, data, datalen, ACK_CHECK_EN);
//   ret |= i2c_master_stop(cmd);
//   ret = i2c_master_cmd_begin(I2C_TAS5805M_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
//   // Check if ret is OK
//   if (ret != ESP_OK)
//   {
//     ESP_LOGE(TAG, "Error during I2C transmission: %s", esp_err_to_name(ret));
//   }
//   i2c_cmd_link_delete(cmd);
//   return ret;
// }

// esp_err_t tas5805m::getGain(uint8_t* volume)
// {
// }

// // 0-255, where 0 = 0 Db, 255 = -15.5 Db
// esp_err_t tas5805m::setGain(uint8_t value)
// {
// }

// esp_err_t tas5805m::getVolume(uint8_t* volume)
// {
// }

// // 0-255, where 0 = 0 Db, 255 = mute
// esp_err_t tas5805m::setVolume(uint8_t value)
// {
// }