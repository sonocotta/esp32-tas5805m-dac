#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "esp_log.h"

#include "driver/i2c.h"
#include "tas5805m.h"
#include "tas5805m_cfg.h"
#include "../eq/tas5805m_eq.h"

#if defined(CONFIG_TAS5805M_DSP_STEREO)
  #pragma message("tas5805m_2.0+basic config is used")
  #include "../startup/tas5805m_2.0+basic.h"
#elif defined(CONFIG_TAS5805M_DSP_MONO)
  #pragma message("tas5805m_1.0+basic config is used")
  #include "../startup/tas5805m_1.0+basic.h"
#elif defined(CONFIG_TAS5805M_DSP_SUBWOOFER_40)
  #pragma message("tas5805m_0.1+eq_40Hz_cutoff config is used")
  #include "../startup/tas5805m_0.1+eq_40Hz_cutoff.h"
#elif defined(CONFIG_TAS5805M_DSP_SUBWOOFER_60)
  #pragma message("tas5805m_0.1+eq_60Hz_cutoff config is used")
  #include "../startup/tas5805m_0.1+eq_60Hz_cutoff.h"
#elif defined(CONFIG_TAS5805M_DSP_SUBWOOFER_100)
  #pragma message("tas5805m_0.1+eq_100Hz_cutoff config is used")
  #include "../startup/tas5805m_0.1+eq_100Hz_cutoff.h"
#elif defined(CONFIG_TAS5805M_DSP_BIAMP)
  #pragma message("tas5805m_1.1+eq_60Hz_cutoff+mono config is used")
  #include "../startup/tas5805m_1.1+eq_60Hz_cutoff+mono.h"
#elif defined(CONFIG_TAS5805M_DSP_BIAMP_60_LEFT)
  #pragma message("tas5805m_1.1+eq_60Hz_cutoff+left config is used")
  #include "../startup/tas5805m_1.1+eq_60Hz_cutoff+left.h"
#elif defined(CONFIG_TAS5805M_DSP_BIAMP_60_RIGHT)
  #pragma message("tas5805m_1.1+eq_60Hz_cutoff+right config is used")
  #include "../startup/tas5805m_1.1+eq_60Hz_cutoff+right.h"
#elif defined(CONFIG_TAS5805M_DSP_STEREO_LOUDNESS_1)
  #pragma message("tas5805m_2.0+eq(+9db_20Hz)(-1Db_500Hz)(+3Db_8kHz)(+3Db_15kHz) config is used")
  #include "../startup/custom/tas5805m_2.0+eq(+9db_20Hz)(-1Db_500Hz)(+3Db_8kHz)(+3Db_15kHz).h"
#elif defined(CONFIG_TAS5805M_DSP_STEREO_LOUDNESS_2)
  #pragma message("tas5805m_2.0+eq(+9db_20Hz)(-3Db_500Hz)(+3Db_8kHz)(+3Db_15kHz) config is used")
  #include "../startup/custom/tas5805m_2.0+eq(+9db_20Hz)(-3Db_500Hz)(+3Db_8kHz)(+3Db_15kHz).h"
#elif defined(CONFIG_TAS5805M_DSP_STEREO_LOUDNESS_3)
  #pragma message("tas5805m_2.0+eq(+12db_30Hz)(-3Db_500Hz)(+3Db_8kHz)(+3Db_15kHz) config is used")
  #include "../startup/custom/tas5805m_2.0+eq(+12db_30Hz)(-3Db_500Hz)(+3Db_8kHz)(+3Db_15kHz).h"
#else
  #pragma message("tas5805m_2.0+minimal config is used")
  #include "../startup/tas5805m_2.0+minimal.h"
#endif

static const char *TAG = "TAS5805";

#define TAS5805M_ASSERT(a, format, b, ...) \
  if ((a) != 0)                            \
  {                                        \
    ESP_LOGE(TAG, format, ##__VA_ARGS__);  \
    return b;                              \
  }

#define TAS5805M_SET_BOOK_AND_PAGE(BOOK, PAGE) \
    do { \
      tas5805m_write_byte(TAS5805M_REG_PAGE_SET, TAS5805M_REG_PAGE_ZERO); \
      tas5805m_write_byte(TAS5805M_REG_BOOK_SET, BOOK);                   \
      tas5805m_write_byte(TAS5805M_REG_PAGE_SET, PAGE);                   \
    } while (0)

TAS5805_STATE tas5805m_state = {
    .is_muted = false,
    .state = TAS5805M_CTRL_PLAY,
    .eq_gain = { 0 },                  // todo: can be redefined in startup sequence
    .mixer_mode = MIXER_UNKNOWN,        // todo: can be redefined in startup sequence
};

/* Helper Functions */
// Reading of TAS5805M-Register
esp_err_t tas5805m_read_byte(uint8_t register_name, uint8_t *data)
{

  int ret;
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, TAS5805M_ADDRESS << 1 | WRITE_BIT, ACK_CHECK_EN);
  i2c_master_write_byte(cmd, register_name, ACK_CHECK_EN);
  i2c_master_stop(cmd);
  ret = i2c_master_cmd_begin(I2C_TAS5805M_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
  i2c_cmd_link_delete(cmd);

  if (ret != ESP_OK)
  {
    ESP_LOGW(TAG, "%s: I2C error %s", __func__, esp_err_to_name(ret));
  }

  vTaskDelay(1 / portTICK_RATE_MS);
  cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, TAS5805M_ADDRESS << 1 | READ_BIT, ACK_CHECK_EN);
  i2c_master_read_byte(cmd, data, NACK_VAL);
  i2c_master_stop(cmd);
  ret = i2c_master_cmd_begin(I2C_TAS5805M_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
  i2c_cmd_link_delete(cmd);

  ESP_LOGV(TAG, "%s: 0x%02x = 0x%02x", __func__, register_name, *data);

  return ret;
}

// Writing of TAS5805M-Register
esp_err_t tas5805m_write_byte(uint8_t register_name, uint8_t value)
{
  ESP_LOGV(TAG, "%s: 0x%02x <- 0x%02x", __func__, register_name, value);
  int ret = 0;
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, TAS5805M_ADDRESS << 1 | WRITE_BIT, ACK_CHECK_EN);
  i2c_master_write_byte(cmd, register_name, ACK_CHECK_EN);
  i2c_master_write_byte(cmd, value, ACK_CHECK_EN);
  i2c_master_stop(cmd);

  ret = i2c_master_cmd_begin(I2C_TAS5805M_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);

  // Check if ret is OK
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG, "%s: error during I2C transmission: %s", __func__, esp_err_to_name(ret));
  }

  i2c_cmd_link_delete(cmd);

  return ret;
}

esp_err_t tas5805m_write_bytes(uint8_t *reg,
                               int regLen, uint8_t *data, int datalen)
{
  int ret = ESP_OK;
  ESP_LOGV(TAG, "%s: 0x%02x <- [%d] bytes", __func__, *reg, datalen);
  for (int i = 0; i < datalen; i++)
  {
    ESP_LOGV(TAG, "%s: 0x%02x", __func__, data[i]);
  }

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
    ESP_LOGE(TAG, "%s: Error during I2C transmission: %s", __func__, esp_err_to_name(ret));
  }

  i2c_cmd_link_delete(cmd);

  return ret;
}

static esp_err_t tas5805m_transmit_registers(const tas5805m_cfg_reg_t *conf_buf, int size)
{
  int i = 0;
  esp_err_t ret = ESP_OK;
  ESP_LOGV(TAG, "%s enter", __func__);
  while (i < size)
  {
    switch (conf_buf[i].offset)
    {
    case CFG_META_SWITCH:
      // Used in legacy applications.  Ignored here.
      break;
    case CFG_META_DELAY:
      vTaskDelay(conf_buf[i].value / portTICK_RATE_MS);
      break;
    case CFG_META_BURST:
      ret = tas5805m_write_bytes((unsigned char *)(&conf_buf[i + 1].offset), 1,
                                 (unsigned char *)(&conf_buf[i + 1].value), conf_buf[i].value);
      i += (conf_buf[i].value / 2) + 1;
      break;
    case CFG_END_1:
      if (CFG_END_2 == conf_buf[i + 1].offset &&
          CFG_END_3 == conf_buf[i + 2].offset)
      {
        ESP_LOGD(TAG, "%s: End of tas5805m reg: %d", __func__, i);
      }
      break;
    default:
      ret = tas5805m_write_bytes((unsigned char *)(&conf_buf[i].offset), 1,
                                 (unsigned char *)(&conf_buf[i].value), 1);
      ESP_LOGV(TAG, "\t0x%02x <- 0x%02x", conf_buf[i].offset, conf_buf[i].value);
      break;
    }
    i++;
  }
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG, "%s: Fail to load configuration to tas5805m", __func__);
    return ESP_FAIL;
  }
  ESP_LOGD(TAG, "%s leave; wrote %d registers", __func__, i);
  return ret;
}

/* Public API */
// Inits the TAS5805M
esp_err_t tas5805m_init()
{
  /// i2c_master_init();

  /* Register the PDN pin as output and write 1 to enable the TAS chip */
  gpio_config_t io_conf;
  io_conf.intr_type = GPIO_INTR_DISABLE;
  io_conf.mode = GPIO_MODE_OUTPUT;
  io_conf.pin_bit_mask = TAS5805M_GPIO_PDN_MASK;
  io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
  ESP_LOGI(TAG, "%s: Running PWR ON sequence on pin: %d", __func__, TAS5805M_GPIO_PDN);
  gpio_config(&io_conf);
  gpio_set_level(TAS5805M_GPIO_PDN, 0);
  vTaskDelay(20 / portTICK_RATE_MS);
  gpio_set_level(TAS5805M_GPIO_PDN, 1);
  vTaskDelay(100 / portTICK_RATE_MS);
  //
  // I2S needs to be initialized before this happens
  //
  // i2s_driver_start(TAS5805M_I2S_NUM);
  // Once I2S clocks are stable, set the device into HiZ state and enable DSP via the I2C control port.
  ESP_ERROR_CHECK(tas5805m_set_state(TAS5805M_CTRL_HI_Z));
  vTaskDelay(20 / portTICK_RATE_MS);
  // 5. Wait 5ms at least. Then initialize the DSP Coefficient, then set the device to Play state.
  int ret = tas5805m_transmit_registers(
      tas5805m_registers,
      sizeof(tas5805m_registers) / sizeof(tas5805m_registers[0]));
  ESP_ERROR_CHECK(tas5805m_set_state(TAS5805M_CTRL_PLAY));

  TAS5805M_ASSERT(ret, "Fail to initialize tas5805m PA", ESP_FAIL);
  return ret;
}

// Setting the Volume [0..255], 0 is mute, 255 is full blast
esp_err_t tas5805m_set_volume(uint8_t vol)
{
  esp_err_t ret = ESP_OK;
  if (vol < TAS5805M_VOLUME_MIN)
  {
    vol = TAS5805M_VOLUME_MIN;
  }
  if (vol > TAS5805M_VOLUME_MAX)
  {
    vol = TAS5805M_VOLUME_MAX;
  }

  ret = tas5805m_write_byte(TAS5805M_DIG_VOL_CTRL, vol);
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG, "%s: Error during I2C transmission: %s", __func__, esp_err_to_name(ret));
  }

  return ret;
}

// Getting the Volume [0..255], 0 is mute, 255 is full blast
esp_err_t tas5805m_get_volume(uint8_t *vol)
{
  esp_err_t ret = ESP_OK;
  ret = tas5805m_read_byte(TAS5805M_DIG_VOL_CTRL, vol);
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG, "%s: Error during I2C transmission: %s", __func__, esp_err_to_name(ret));
  }
  return ret;
}

// Setting the Volume [0..124], where 100 is 0 Db, 0 is mute
esp_err_t tas5805m_set_volume_pct(uint8_t vol)
{
  esp_err_t ret = ESP_OK;
  if (vol < TAS5805M_VOLUME_PCT_MIN)
    vol = TAS5805M_VOLUME_PCT_MIN;
  if (vol > TAS5805M_VOLUME_PCT_MAX)
    vol = TAS5805M_VOLUME_PCT_MAX;

  // Convert to 0..255
  uint volume = vol == 0 ? TAS5805M_VOLUME_MAX : ( -2 * vol + 248 );
  return tas5805m_set_volume(volume);
}

// Get the Volume [0..124], where 100 is 0 Db, 0 is mute
esp_err_t tas5805m_get_volume_pct(uint8_t *vol)
{
  uint8_t volume;
  esp_err_t ret = tas5805m_get_volume(&volume);
  
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG, "%s: Error during I2C transmission: %s", __func__, esp_err_to_name(ret));
    return ret;
  }

  *vol = volume >= 248 ? 0 : (248 - volume) / 2;
  return ret;
}

esp_err_t tas5805m_deinit(void)
{
  gpio_set_level(TAS5805M_GPIO_PDN, 0);
  return ESP_OK;
}

esp_err_t tas5805m_get_mute(bool *enabled)
{
  *enabled = tas5805m_state.is_muted;
  return ESP_OK;
}

esp_err_t tas5805m_set_mute(bool enable)
{
  tas5805m_state.is_muted = enable;
  return tas5805m_set_state(enable ? TAS5805M_CTRL_MUTE : TAS5805M_CTRL_PLAY);
}

esp_err_t tas5805m_get_state(TAS5805M_CTRL_STATE *state)
{
  *state = tas5805m_state.state;
  return ESP_OK;
}

esp_err_t tas5805m_set_state(TAS5805M_CTRL_STATE state)
{
  ESP_LOGD(TAG, "%s: Setting state to %d", __func__, state);
  tas5805m_state.state = state;
  return tas5805m_write_byte(TAS5805M_DEVICE_CTRL_2, state);
}

esp_err_t tas5805m_get_dac_mode(TAS5805M_DAC_MODE *mode)
{
    uint8_t current_value;
    esp_err_t err = tas5805m_read_byte(TAS5805M_DEVICE_CTRL_1, &current_value);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: Error during I2C transmission: %s", __func__, esp_err_to_name(err));
        return err;
    }

    if (current_value & (1 << 2)) {
        *mode = TAS5805M_DAC_MODE_PBTL;
    } else {
        *mode = TAS5805M_DAC_MODE_BTL;
    }

    return ESP_OK;
}

esp_err_t tas5805m_set_dac_mode(TAS5805M_DAC_MODE mode)
{
    ESP_LOGD(TAG, "%s: Setting DAC mode to %d", __func__, mode);

    // Read the current value of the register
    uint8_t current_value;
    esp_err_t err = tas5805m_read_byte(TAS5805M_DEVICE_CTRL_1, &current_value);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: Error during I2C transmission: %s", __func__, esp_err_to_name(err));
        return err;
    }

    // Update bit 2 based on the mode
    if (mode == TAS5805M_DAC_MODE_PBTL) {
        current_value |= (1 << 2);  // Set bit 2 to 1 (PBTL mode)
    } else {
        current_value &= ~(1 << 2); // Clear bit 2 to 0 (BTL mode)
    }

    // Write the updated value back to the register
    int ret = tas5805m_write_byte(TAS5805M_DEVICE_CTRL_1, current_value);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "%s: Error during I2C transmission: %s", __func__, esp_err_to_name(ret));
    }

    return ret;
}

esp_err_t tas5805m_get_eq(bool *enabled)
{
  uint8_t value = 0;
  esp_err_t err = tas5805m_read_byte(TAS5805M_DSP_MISC, &value);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%s: Error during I2C transmission: %s", __func__, esp_err_to_name(err));
    return err;
  }

  if (value & 0x01) {
    *enabled = false;
  } else {
    *enabled = true;
  }

  return ESP_OK;
}

esp_err_t tas5805m_set_eq(bool enable)
{
  ESP_LOGD(TAG, "%s: Setting EQ to %d", __func__, enable);
  return tas5805m_write_byte(TAS5805M_DSP_MISC, enable ? TAS5805M_CTRL_EQ_ON : TAS5805M_CTRL_EQ_OFF);
}

esp_err_t tas5805m_get_eq_gain(int band, int *gain)
{
  *gain = tas5805m_state.eq_gain[band];
  return ESP_OK;
}

esp_err_t tas5805m_set_eq_gain(int band, int gain)
{
  if (band < 0 || band >= TAS5805M_EQ_BANDS)
  {
    ESP_LOGE(TAG, "%s: Invalid band %d", __func__, band);
    return ESP_ERR_INVALID_ARG;
  }

  if (gain < TAS5805M_EQ_MIN_DB || gain > TAS5805M_EQ_MAX_DB)
  {
    ESP_LOGE(TAG, "%s: Invalid gain %d", __func__, gain);
    return ESP_ERR_INVALID_ARG;
  }

  int current_page = 0; 
  int ret = ESP_OK;
  ESP_LOGD(TAG, "%s: Setting EQ band %d (%d Hz) to gain %d", __func__, band, tas5805m_eq_bands[band], gain);

  int x = gain + TAS5805M_EQ_MAX_DB;                                 
  int y = band * TAS5805M_EQ_KOEF_PER_BAND * TAS5805M_EQ_REG_PER_KOEF; 
                                                                      
  for (int i = 0; i < TAS5805M_EQ_KOEF_PER_BAND * TAS5805M_EQ_REG_PER_KOEF; i++) 
  { 
      const reg_sequence_eq *reg_value = &tas5805m_eq_registers[x][y + i]; 
      if (reg_value == NULL) {                                        
          ESP_LOGW(TAG, "%s: NULL pointer encountered at row[%d]", __func__, y + i); 
          continue;                                                   
      }                                                               
                                                                      
      if (reg_value->page != current_page) {                          
          current_page = reg_value->page;                             
          TAS5805M_SET_BOOK_AND_PAGE(TAS5805M_REG_BOOK_EQ, reg_value->page); 
      }                                                               
                                                                      
      ESP_LOGV(TAG, "+ %d: w 0x%x 0x%x", i, reg_value->offset, reg_value->value);
      ret = ret | tas5805m_write_byte(reg_value->offset, reg_value->value);
      if (ret != ESP_OK) { 
          ESP_LOGE(TAG, "%s: Error writing to register 0x%x", __func__, reg_value->offset); 
      
      }          
  }   
  
  tas5805m_state.eq_gain[band] = gain;
                                                                      
  TAS5805M_SET_BOOK_AND_PAGE(TAS5805M_REG_BOOK_CONTROL_PORT, TAS5805M_REG_PAGE_ZERO); 
  return ret;
}

esp_err_t tas5805m_get_modulation_mode(TAS5805M_MOD_MODE *mode, TAS5805M_SW_FREQ *freq, TAS5805M_BD_FREQ *bd_freq)
{
  // Read the current value of the register
  uint8_t current_value;
  esp_err_t err = tas5805m_read_byte(TAS5805M_DEVICE_CTRL_1, &current_value);
  if (err != ESP_OK) {
      ESP_LOGE(TAG, "%s: Error during I2C transmission: %s", __func__, esp_err_to_name(err));
      return err;
  }

  // Extract bits 0-1
  *mode = (current_value & 0b00000011);
  // Extract bits 4-6
  *freq = (current_value & 0b01110000);

  // Read the BD frequency
  err = tas5805m_read_byte(TAS5805M_ANA_CTRL, &current_value);
  if (err != ESP_OK) {
      ESP_LOGE(TAG, "%s: Error during I2C transmission: %s", __func__, esp_err_to_name(err));
      return err;
  }

  *bd_freq = (current_value & 0b01100000);
  return ESP_OK;
}

esp_err_t tas5805m_set_modulation_mode(TAS5805M_MOD_MODE mode, TAS5805M_SW_FREQ freq, TAS5805M_BD_FREQ bd_freq)
{
  ESP_LOGD(TAG, "%s: Setting modulation to %d, FSW: %d, Class-D bandwidth control: %d", __func__, mode, freq, bd_freq);

  // Read the current value of the register
  uint8_t current_value;
  esp_err_t err = tas5805m_read_byte(TAS5805M_DEVICE_CTRL_1, &current_value);
  if (err != ESP_OK) {
      ESP_LOGE(TAG, "%s: Error during I2C transmission: %s", __func__, esp_err_to_name(err));
      return err;
  }

  // Clear bits 0-1 and 4-6
  current_value &= ~((0x07 << 4) | (0x03 << 0));
  // Update bit 0-1 based on the mode
  current_value |= mode & 0b00000011;  // Set bits 0-1
  // Update bits 4-6 based on sw freq
  current_value |= freq & 0b01110000;  // Set bits 4-6
  
  // Write the updated value back to the register
  int ret = tas5805m_write_byte(TAS5805M_DEVICE_CTRL_1, current_value);
  if (ret != ESP_OK) {
      ESP_LOGE(TAG, "%s: Error during I2C transmission: %s", __func__, esp_err_to_name(ret));
  } 

  // Set the BD frequency
  ret = tas5805m_write_byte(TAS5805M_ANA_CTRL, bd_freq);
  if (ret != ESP_OK) {
      ESP_LOGE(TAG, "%s: Error during I2C transmission: %s", __func__, esp_err_to_name(ret));
  } 

  return ret;
}

esp_err_t tas5805m_get_again(uint8_t *gain)
{
  int ret = ESP_OK;
  ret = tas5805m_read_byte(TAS5805M_AGAIN, gain);
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG, "%s: Error during I2C transmission: %s", __func__, esp_err_to_name(ret));
  }
  return ret;
}

esp_err_t tas5805m_set_again(uint8_t gain)
{
  // Gain is inverted!
  if (gain < TAS5805M_MAX_GAIN || gain > TAS5805M_MIN_GAIN)
  {
    ESP_LOGE(TAG, "%s: Invalid gain %d", __func__, gain);
    return ESP_ERR_INVALID_ARG;
  }

  uint8_t value = tas5805m_again[gain];
  int ret = tas5805m_write_byte(TAS5805M_AGAIN, value);
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG, "%s: Error during I2C transmission: %s", __func__, esp_err_to_name(ret));
  }

  return ret;
}

esp_err_t tas5805m_get_mixer_mode(TAS5805M_MIXER_MODE *mode)
{
  *mode = tas5805m_state.mixer_mode;
  return ESP_OK;
}

esp_err_t tas5805m_set_mixer_mode(TAS5805M_MIXER_MODE mode)
{
  ESP_LOGD(TAG, "%s: Setting mixer mode to %d", __func__, mode);
  
  uint32_t mixer_l_to_l, mixer_r_to_r, mixer_l_to_r, mixer_r_to_l;
  int ret = ESP_OK;

  switch (mode)
  {
  case MIXER_STEREO:
    mixer_l_to_l = TAS5805M_MIXER_VALUE_0DB;
    mixer_r_to_r = TAS5805M_MIXER_VALUE_MUTE;
    mixer_l_to_r = TAS5805M_MIXER_VALUE_MUTE;
    mixer_r_to_l = TAS5805M_MIXER_VALUE_0DB;
    break;

  case MIXER_STEREO_INVERSE:
    mixer_l_to_l = TAS5805M_MIXER_VALUE_MUTE;
    mixer_r_to_r = TAS5805M_MIXER_VALUE_0DB;
    mixer_l_to_r = TAS5805M_MIXER_VALUE_0DB;
    mixer_r_to_l = TAS5805M_MIXER_VALUE_MUTE;
    break;

  case MIXER_MONO:
    mixer_l_to_l = TAS5805M_MIXER_VALUE_MINUS6DB;
    mixer_r_to_r = TAS5805M_MIXER_VALUE_MINUS6DB;
    mixer_l_to_r = TAS5805M_MIXER_VALUE_MINUS6DB;
    mixer_r_to_l = TAS5805M_MIXER_VALUE_MINUS6DB;
    break;

  case MIXER_LEFT:
    mixer_l_to_l = TAS5805M_MIXER_VALUE_0DB;
    mixer_r_to_r = TAS5805M_MIXER_VALUE_MUTE;
    mixer_l_to_r = TAS5805M_MIXER_VALUE_0DB;
    mixer_r_to_l = TAS5805M_MIXER_VALUE_MUTE;
    break;

  case MIXER_RIGHT:
    mixer_l_to_l = TAS5805M_MIXER_VALUE_MUTE;
    mixer_r_to_r = TAS5805M_MIXER_VALUE_0DB;
    mixer_l_to_r = TAS5805M_MIXER_VALUE_MUTE;
    mixer_r_to_l = TAS5805M_MIXER_VALUE_0DB;
    break;

  default:
    ESP_LOGE(TAG, "%s: Invalid mixer mode %d", __func__, mode);
    return ESP_ERR_INVALID_ARG;
  }
    
  TAS5805M_SET_BOOK_AND_PAGE(TAS5805M_REG_BOOK_5, TAS5805M_REG_BOOK_5_MIXER_PAGE);
  ret = tas5805m_write_byte(TAS5805M_REG_LEFT_TO_LEFT_GAIN, mixer_l_to_l);
  if (ret != 0) {
    ESP_LOGE(TAG, "Failed to write register %d: %d", TAS5805M_REG_LEFT_TO_LEFT_GAIN, ret);
    return ret;
  }

  ret = tas5805m_write_byte(TAS5805M_REG_RIGHT_TO_RIGHT_GAIN, mixer_r_to_r);
  if (ret != 0) {
    ESP_LOGE(TAG, "Failed to write register %d: %d", TAS5805M_REG_RIGHT_TO_RIGHT_GAIN, ret);
    return ret;
  }

  ret = tas5805m_write_byte(TAS5805M_REG_LEFT_TO_RIGHT_GAIN, mixer_l_to_r);
  if (ret != 0) {
    ESP_LOGE(TAG, "Failed to write register %d: %d", TAS5805M_REG_LEFT_TO_RIGHT_GAIN, ret);
    return ret;
  }

  ret = tas5805m_write_byte(TAS5805M_REG_RIGHT_TO_LEFT_GAIN, mixer_r_to_l);
  if (ret != 0) {
    ESP_LOGE(TAG, "Failed to write register %d: %d", TAS5805M_REG_RIGHT_TO_LEFT_GAIN, ret);
    return ret;
  }

  tas5805m_state.mixer_mode = mode;
  TAS5805M_SET_BOOK_AND_PAGE(TAS5805M_REG_BOOK_CONTROL_PORT, TAS5805M_REG_PAGE_ZERO); 
  return ret;
}


esp_err_t tas5805m_get_fs_freq(TAS5805M_FS_FREQ *freq)
{
  uint8_t reg_value;
  int ret = tas5805m_read_byte(TAS5805M_FS_MON, &reg_value);
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG, "%s: Error during I2C transmission: %s", __func__, esp_err_to_name(ret));
  } else {
    *freq = (TAS5805M_FS_FREQ)reg_value;
  }
  return ret;
}

esp_err_t tas5805m_get_bck_ratio(uint8_t *ratio)
{
  uint8_t reg_value;
  int ret = tas5805m_read_byte(TAS5805M_BCK_MON, &reg_value);
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG, "%s: Error during I2C transmission: %s", __func__, esp_err_to_name(ret));
  } else {
    *ratio = reg_value;
  }
  return ret;
}

esp_err_t tas5805m_get_power_state(TAS5805M_CTRL_STATE *state)
{
  uint8_t reg_value;
  int ret = tas5805m_read_byte(TAS5805M_POWER_STATE, &reg_value);
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG, "%s: Error during I2C transmission: %s", __func__, esp_err_to_name(ret));
  } else {
    *state = (TAS5805M_CTRL_STATE)reg_value;
  }
  return ret;
}

esp_err_t tas5805m_get_automute_state(bool *is_r_muted, bool *is_l_muted)
{
  uint8_t reg_value;
  int ret = tas5805m_read_byte(TAS5805M_AUTOMUTE_STATE, &reg_value);
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG, "%s: Error during I2C transmission: %s", __func__, esp_err_to_name(ret));
  } else {
    *is_l_muted = (reg_value & 0x01);
    *is_r_muted = (reg_value & 0x02);
  }
  return ret;
}

esp_err_t tas5805m_clear_faults()
{
  ESP_LOGD(TAG, "%s: Clearing faults", __func__);
  int ret = tas5805m_write_byte(TAS5805M_FAULT_CLEAR, TAS5805M_ANALOG_FAULT_CLEAR);
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG, "%s: Error during I2C transmission: %s", __func__, esp_err_to_name(ret));
  }
  return ret;
}

esp_err_t tas5805m_get_faults(TAS5805M_FAULT *fault)
{
  int ret = ESP_OK;

  ret = tas5805m_read_byte(TAS5805M_CHAN_FAULT, &(fault->err0));
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG, "%s: Error during I2C transmission: %s", __func__, esp_err_to_name(ret));
    return ret;
  }

  ret = tas5805m_read_byte(TAS5805M_GLOBAL_FAULT1, &(fault->err1));
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG, "%s: Error during I2C transmission: %s", __func__, esp_err_to_name(ret));
    return ret;
  }

  ret = tas5805m_read_byte(TAS5805M_GLOBAL_FAULT2, &(fault->err2));
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG, "%s: Error during I2C transmission: %s", __func__, esp_err_to_name(ret));
    return ret;
  }

  ret= tas5805m_read_byte(TAS5805M_OT_WARNING, &(fault->ot_warn));
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG, "%s: Error during I2C transmission: %s", __func__, esp_err_to_name(ret));
    return ret;
  }

  return ret;
}

void tas5805m_decode_faults(TAS5805M_FAULT fault)
{
  if (fault.err0) {
    if (fault.err0 & (1 << 0))  
        ESP_LOGW(TAG, "%s: Right channel over current fault", __func__);

    if (fault.err0 & (1 << 1))
        ESP_LOGW(TAG, "%s: Left channel over current fault", __func__);

    if (fault.err0 & (1 << 2)) 
        ESP_LOGW(TAG, "%s: Right channel DC fault", __func__);

    if (fault.err0 & (1 << 3))  
        ESP_LOGW(TAG, "%s: Left channel DC fault", __func__);
  }

  if (fault.err1) {
    if (fault.err1 & (1 << 0))  
        ESP_LOGW(TAG, "%s: PVDD UV fault", __func__);

    if (fault.err1 & (1 << 1))
        ESP_LOGW(TAG, "%s: PVDD OV fault", __func__);

    if (fault.err1 & (1 << 2)) 
        ESP_LOGW(TAG, "%s: Clock fault", __func__);

    if (fault.err1 & (1 << 6))  
        ESP_LOGW(TAG, "%s: The recent BQ is written failed", __func__);

    if (fault.err1 & (1 << 7))  
        ESP_LOGW(TAG, "%s: Indicate OTP CRC check error", __func__);
  }

  if (fault.err2) {
    if (fault.err2 & (1 << 0))  
        ESP_LOGW(TAG, "%s: Over temperature shut down fault", __func__);
  }

  if (fault.ot_warn) {
    if (fault.ot_warn & (1 << 2))  
        ESP_LOGW(TAG, "%s: Over temperature warning", __func__);
  }
}

const char *tas5805m_map_fs_freq(TAS5805M_FS_FREQ freq)
{
    switch (freq)
    {
    case FS_FREQ_ERROR:
        return "ERROR";
    case FS_FREQ_8K:
        return "8KHz";
    case FS_FREQ_16K:
        return "16KHz";
    case FS_FREQ_32K:
        return "32KHz";
    case FS_FREQ_48K:
        return "48KHz";
    case FS_FREQ_96K:
        return "96KHz";
    case FS_FREQ_RESERVED:
        return "RESERVED";
    default:
        return "UNKNOWN";
    }
}

const char* tas5805m_map_amp_state(TAS5805M_CTRL_STATE state)
{
    switch (state)
    {
    case TAS5805M_CTRL_PLAY:
        return "PLAY";
    case TAS5805M_CTRL_HI_Z:
        return "HIGH-Z";
    case TAS5805M_CTRL_SLEEP:
        return "SLEEP";
    case TAS5805M_CTRL_MUTE:
        return "MUTE";
    case TAS5805M_CTRL_DEEP_SLEEP:
        return "DEEP SLEEP";
    default:
        return "UNKNOWN";
    }
}