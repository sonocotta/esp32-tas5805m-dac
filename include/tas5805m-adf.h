#ifndef _TAS5805M_ADF_H_
#define _TAS5805M_ADF_H_

#include "tas5805m.h"
#include "audio_hal.h"

#include "esp_err.h"
#include "esp_log.h"
#include "board.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define I2C_MASTER_FREQ_HZ 400000   /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS 1000

   /**
    * @brief Initialize TAS5805 codec chip
    *
    * @param cfg configuration of TAS5805
    *
    * @return
    *     - ESP_OK
    *     - ESP_FAIL
    */
   esp_err_t tas5805m_adf_init();

   /**
    * @brief Deinitialize TAS5805 codec chip
    *
    * @return
    *     - ESP_OK
    *     - ESP_FAIL
    */
   esp_err_t tas5805m_adf_deinit(void);

   /**
    * @brief  Set voice volume
    *
    * @param volume:  voice volume (0~100)
    *
    * @return
    *     - ESP_OK
    *     - ESP_FAIL
    */
   esp_err_t tas5805m_adf_set_volume(int vol);
   
   /**
    * @brief Get voice volume
    *
    * @param[out] *volume:  voice volume (0~100)
    *
    * @return
    *     - ESP_OK
    *     - ESP_FAIL
    */
   esp_err_t tas5805m_adf_get_volume(int *vol);

   /**
    * @brief Set TAS5805 mute or not
    *        Continuously call should have an interval time determined by
    * tas5805m_adf_set_mute_fade()
    *
    * @param enable enable(1) or disable(0)
    *
    * @return
    *     - ESP_FAIL Parameter error
    *     - ESP_OK   Success
    */
   esp_err_t tas5805m_adf_set_mute(bool enable);

   /**
    * @brief Get TAS5805 mute status
    *
    *  @return
    *     - ESP_FAIL Parameter error
    *     - ESP_OK   Success
    */
   esp_err_t tas5805m_adf_get_mute(bool *enabled);
   
   /**
    * @brief Set the state of the TAS5805M
    *
    * @param state: The state to set
    *
    * @return
    *     - ESP_OK
    *     - ESP_FAIL
    */
   esp_err_t tas5805m_adf_ctrl(audio_hal_codec_mode_t mode,
      audio_hal_ctrl_t ctrl_state);
      
   esp_err_t tas5805m_adf_config_iface(audio_hal_codec_mode_t mode,
         audio_hal_codec_i2s_iface_t *iface);

#ifdef __cplusplus
}
#endif

#endif