#include "tas5805m-adf.h"
#include "tas5805m.h"

#include "esp_log.h"
#include "driver/i2s.h"
#include "driver/i2c.h"

/* Default I2C config */
static i2c_config_t i2c_cfg = {
    .mode = I2C_MODE_MASTER,
    .sda_pullup_en = GPIO_PULLUP_ENABLE,
    .scl_pullup_en = GPIO_PULLUP_ENABLE,
    .master.clk_speed = I2C_MASTER_FREQ_HZ,
};

/*
 * Operate fuction of PA
 */
audio_hal_func_t AUDIO_CODEC_TAS5805M_DEFAULT_HANDLE = {
    .audio_codec_initialize = tas5805m_adf_init,
    .audio_codec_deinitialize = tas5805m_adf_deinit,
    .audio_codec_ctrl = tas5805m_adf_ctrl,
    .audio_codec_config_iface = tas5805m_adf_config_iface,
    .audio_codec_set_mute = tas5805m_adf_set_mute,
    .audio_codec_set_volume = tas5805m_adf_set_volume,
    .audio_codec_get_volume = tas5805m_adf_get_volume,
    .audio_hal_lock = NULL,
    .handle = NULL,
};

/* Init the I2C Driver */
void i2c_master_init()
{
    int i2c_master_port = I2C_MASTER_NUM;
    ESP_ERROR_CHECK(get_i2c_pins(I2C_NUM_0, &i2c_cfg));
    ESP_ERROR_CHECK(i2c_param_config(i2c_master_port, &i2c_cfg));
    ESP_ERROR_CHECK(i2c_driver_install(i2c_master_port, i2c_cfg.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0));
}

esp_err_t tas5805m_adf_init()
{
    // Init the I2C-Driver
    i2c_master_init();

    return tas5805m_init();
}

esp_err_t tas5805m_adf_deinit(void)
{
    return tas5805m_deinit();
}

esp_err_t tas5805m_adf_set_volume(int vol)
{
    return tas5805m_set_volume_pct(vol);
}

esp_err_t tas5805m_adf_get_volume(int *vol)
{
    return tas5805m_get_volume_pct(vol);
}

esp_err_t tas5805m_adf_set_mute(bool enable)
{
    return tas5805m_set_mute(enable);
}

esp_err_t tas5805m_adf_get_mute(bool *enabled)
{
    return tas5805m_get_mute(enabled);
}

esp_err_t tas5805m_adf_ctrl(audio_hal_codec_mode_t mode,
                        audio_hal_ctrl_t ctrl_state)
{
    // TODO: Implement
    // TAS5805M_HANDLE_t *codec = (TAS5805M_HANDLE_t *)handle;
    // esp_err_t ret = ESP_OK;

    // switch (cmd) {
    //     case AUDIO_CODEC_CTRL_SET_VOLUME: {
    //         uint8_t volume = *(uint8_t *)value;
    //         ret = tas5805m_set_volume(codec, volume);
    //         break;
    //     }
    //     case AUDIO_CODEC_CTRL_START:
    //         ret = tas5805m_power(codec, true);
    //         break;
    //     case AUDIO_CODEC_CTRL_STOP:
    //         ret = tas5805m_power(codec, false);
    //         break;
    //     default:
    //         ESP_LOGW(TAG, "Unsupported command: %d", cmd);
    //         ret = ESP_ERR_NOT_SUPPORTED;
    // }

    // return ret;

    return ESP_OK;
}

esp_err_t tas5805m_adf_config_iface(audio_hal_codec_mode_t mode,
                                audio_hal_codec_i2s_iface_t *iface)
{
    // TODO: Implement
    // TAS5805M_HANDLE_t *codec = (TAS5805M_HANDLE_t *)handle;

    // ESP_LOGI(TAG, "Configuring I2S Interface: bits=%d, fmt=%d", cfg->bits, cfg->fmt);

    // // Update the codec configuration register (example)
    // return tas5805m_set_i2s_format(codec, cfg->bits, cfg->fmt);
    return ESP_OK;
}