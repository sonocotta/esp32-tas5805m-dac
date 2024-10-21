#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "driver/i2c.h"
#include "tas5805m.hpp"

#if defined(TAS5805M_DSP_STEREO)
    #pragma message("tas5805m_2.0+basic config is used")
    #include "startup/tas5805m_2.0+basic.h"
    #elif defined(TAS5805M_DSP_STEREO_DRC_AGL)
    #pragma message("tas5805m_2.0+3-band_drc+agl_-12db config is used")
    #include "startup/tas5805m_2.0+3-band_drc+agl_-12db.h"
    #elif defined(TAS5805M_DSP_STEREO_DRC)
    #pragma message("tas5805m_2.0+3-band_drc config is used")
    #include "startup/tas5805m_2.0+3-band_drc.h"
    #elif defined(TAS5805M_DSP_STEREO_AGL)
    #pragma message("tas5805m_2.0+agl_-12db config is used")
    #include "startup/tas5805m_2.0+agl_-12db.h"
#elif defined(TAS5805M_DSP_MONO)
    #pragma message("tas5805m_1.0+basic config is used")
    #include "startup/tas5805m_1.0+basic.h"
    #elif defined(TAS5805M_DSP_MONO_DRC_AGL)
    #pragma message("tas5805m_1.0+3-band_drc+agl_-12db config is used")
    #include "startup/tas5805m_1.0+3-band_drc+agl_-12db.h"
    #elif defined(TAS5805M_DSP_MONO_DRC)
    #pragma message("tas5805m_1.0+3-band_drc config is used")
    #include "startup/tas5805m_1.0+3-band_drc.h"
    #elif defined(TAS5805M_DSP_MONO_AGL)
    #pragma message("tas5805m_1.0+agl_-12db config is used")
    #include "startup/tas5805m_1.0+agl_-12db.h"
#elif defined(TAS5805M_DSP_SUBWOOFER_100_AGL)
    #pragma message("tas5805m_0.1+eq_100Hz_cutoff+drc config is used")
    #include "startup/tas5805m_0.1+eq_100Hz_cutoff+drc.h" // works: yes // <- purepath (PBTL) subwoofer mode 
    #elif defined(TAS5805M_DSP_SUBWOOFER_40)
    #pragma message("tas5805m_0.1+eq_40Hz_cutoff config is used")
    #include "startup/tas5805m_0.1+eq_40Hz_cutoff.h"
    #elif defined(TAS5805M_DSP_SUBWOOFER_60)
    #pragma message("tas5805m_0.1+eq_60Hz_cutoff config is used")
    #include "startup/tas5805m_0.1+eq_60Hz_cutoff.h"
    #elif defined(TAS5805M_DSP_SUBWOOFER_100)
    #pragma message("tas5805m_0.1+eq_100Hz_cutoff config is used")
    #include "startup/tas5805m_0.1+eq_100Hz_cutoff.h"// works: yes // <- purepath (PBTL) subwoofer mode 
#elif defined(TAS5805M_DSP_BIAMP)
    #pragma message("tas5805m_1.1+eq_60Hz_cutoff+mono config is used")
    #include "startup/tas5805m_1.1+eq_60Hz_cutoff+mono.h"
    #elif defined(TAS5805M_DSP_BIAMP_60_LEFT)
    #pragma message("tas5805m_1.1+eq_60Hz_cutoff+left config is used")
    #include "startup/tas5805m_1.1+eq_60Hz_cutoff+left.h"
    #elif defined(TAS5805M_DSP_BIAMP_60_RIGHT)
    #pragma message("tas5805m_1.1+eq_60Hz_cutoff+right config is used") 
    #include "startup/tas5805m_1.1+eq_60Hz_cutoff+right.h"
#else
    #pragma message("tas5805m_2.0+minimal config is used")
    #include "startup/tas5805m_2.0+minimal.h"        // works: yes // <- purepath minimal
#endif

static const char *TAG = "TAS5805";

#define TAS5805M_ASSERT(a, format, b, ...) \
  if ((a) != 0) {                          \
    ESP_LOGE(TAG, format, ##__VA_ARGS__);  \
    return b;                              \
  }

esp_err_t tas5805m::init()
{
    /* Register the PDN pin as output and write 1 to enable the TAS chip */
    ESP_LOGD(TAG, "init");
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = TAS5805M_GPIO_PDN_MASK;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    ESP_LOGD(TAG, "Power down pin: %d", TAS5805M_GPIO_PDN); 
    gpio_config(&io_conf);
    gpio_set_level(TAS5805M_GPIO_PDN, 0);
    vTaskDelay(20 / portTICK_RATE_MS);
    gpio_set_level(TAS5805M_GPIO_PDN, 1);
    vTaskDelay(200 / portTICK_RATE_MS);

    return ESP_OK;
}

esp_err_t tas5805m::begin()
{
    ESP_LOGD(TAG, "begin");
    esp_err_t ret = ESP_OK;

    ret |= _transmit_registers(
        tas5805m_registers,
        sizeof(tas5805m_registers) / sizeof(tas5805m_registers[0]));

    TAS5805M_ASSERT(ret, "Fail to initialize TAS5805M PA", ESP_FAIL);

    return ret;
}

esp_err_t tas5805m::_transmit_registers(const tas5805m_cfg_reg_t *conf_buf, int size) {
  int i = 0;
  esp_err_t ret = ESP_OK;
  while (i < size) {
    switch (conf_buf[i].offset) {
      case CFG_META_SWITCH:
        // Used in legacy applications.  Ignored here.
        break;
      case CFG_META_DELAY:
        vTaskDelay(conf_buf[i].value / portTICK_RATE_MS);
        break;
      case CFG_META_BURST:
        ret = _write_bytes((unsigned char *)(&conf_buf[i + 1].offset), 1,
                                   (unsigned char *)(&conf_buf[i + 1].value), conf_buf[i].value);
        i += (conf_buf[i].value / 2) + 1;
        break;
      case CFG_END_1:
        if (CFG_END_2 == conf_buf[i + 1].offset &&
            CFG_END_3 == conf_buf[i + 2].offset) {
          ESP_LOGI(TAG, "End of tas5805m reg: %d\n", i);
        }
        break;
      default:
        ret = _write_bytes((unsigned char *)(&conf_buf[i].offset), 1,
                                   (unsigned char *)(&conf_buf[i].value), 1);
        break;
    }
    i++;
  }
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Fail to load configuration to tas5805m");
    return ESP_FAIL;
  }
  ESP_LOGI(TAG, "%s:  write %d registers successfully", __FUNCTION__, i);
  return ret;
}

esp_err_t tas5805m::getFaultState(uint8_t *h70, uint8_t *h71, uint8_t *h72)
{
    esp_err_t ret = ESP_OK;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, TAS5805M_ADDRESS << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, TAS5805M_CHAN_FAULT_REGISTER, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_TAS5805M_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "I2C ERROR");
    }

    vTaskDelay(1 / portTICK_RATE_MS);
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, TAS5805M_ADDRESS << 1 | READ_BIT, ACK_CHECK_EN);
    i2c_master_read_byte(cmd, h70, ACK_VAL);
    i2c_master_read_byte(cmd, h71, ACK_VAL);
    i2c_master_read_byte(cmd, h72, NACK_VAL);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_TAS5805M_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

esp_err_t tas5805m::clearFaultState() 
{
    return _write_byte(TAS5805M_FAULT_CLEAR_REGISTER, (uint8_t)0b10000000);
}

esp_err_t tas5805m::_read(uint8_t register_name, uint8_t *data_rd, size_t size)
{
    if (size == 0)
    {
        return ESP_OK;
    }

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (TAS5805M_ADDRESS << 1) | READ_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, register_name, ACK_CHECK_EN);

    if (size > 1)
    {
        i2c_master_read(cmd, data_rd, size - 1, ACK_VAL);
    }
    i2c_master_read_byte(cmd, data_rd + size - 1, NACK_VAL);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_TAS5805M_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    return ret;
}


esp_err_t tas5805m::_read_byte(uint8_t register_name, uint8_t *data)
{
    
    esp_err_t ret = ESP_OK;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, TAS5805M_ADDRESS << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, register_name, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_TAS5805M_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "I2C ERROR");
    }

    vTaskDelay(1 / portTICK_RATE_MS);
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, TAS5805M_ADDRESS << 1 | READ_BIT, ACK_CHECK_EN);
    i2c_master_read_byte(cmd, data, NACK_VAL);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_TAS5805M_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    
    return ret;
}


esp_err_t tas5805m::_write_byte(uint8_t register_name, uint8_t value)
{
    esp_err_t ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, TAS5805M_ADDRESS << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, register_name, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, value, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_TAS5805M_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    return ret;
}

esp_err_t tas5805m::_write_bytes(uint8_t *reg, int regLen, uint8_t *data, int datalen)
{
  esp_err_t ret = ESP_OK;

  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  ret |= i2c_master_start(cmd);
  ret |= i2c_master_write_byte(cmd, TAS5805M_ADDRESS << 1 | WRITE_BIT, ACK_CHECK_EN);
  ret |= i2c_master_write(cmd, reg, regLen, ACK_CHECK_EN);
  ret |= i2c_master_write(cmd, data, datalen, ACK_CHECK_EN);
  ret |= i2c_master_stop(cmd);
  ret = i2c_master_cmd_begin(I2C_TAS5805M_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);

  // Check if ret is OK
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG, "Error during I2C transmission: %s", esp_err_to_name(ret));
  }

  i2c_cmd_link_delete(cmd);

  return ret;
}

esp_err_t tas5805m::getGain(uint8_t* volume)
{
    uint8_t value = 0;
    esp_err_t ret = _read_byte(TAS5805M_AGAIN_REGISTER, &value);
    *volume = value << 3;
    return ret;
}

// 0-255, where 0 = 0 Db, 255 = -15.5 Db
esp_err_t tas5805m::setGain(uint8_t value)
{
    uint8_t volume = value >> 3;
    return _write_byte(TAS5805M_AGAIN_REGISTER, volume);
}

esp_err_t tas5805m::getVolume(uint8_t* volume)
{
    uint8_t value = 0;
    esp_err_t ret = _read_byte(TAS5805M_DIG_VOL_CTRL_REGISTER, &value);
    *volume = value;
    return ret;
}

// 0-255, where 0 = 0 Db, 255 = mute
esp_err_t tas5805m::setVolume(uint8_t value)
{
    uint8_t volume = value;
    return _write_byte(TAS5805M_DIG_VOL_CTRL_REGISTER, volume);
}
