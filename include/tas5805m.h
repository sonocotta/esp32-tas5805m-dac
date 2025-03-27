#ifndef _TAS5805M_H_
#define _TAS5805M_H_

#include <stdbool.h>
#include "esp_err.h"
#include "esp_log.h"

#include "../eq/tas5805m_eq.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define I2C_MASTER_NUM I2C_NUM_0   /*!< I2C port number for master dev */
#define I2C_TAS5805M_MASTER_NUM I2C_MASTER_NUM /*!< I2C port number for master dev */

#define I2C_MASTER_FREQ_HZ 400000   /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS 1000
                                             
typedef enum {
    TAS5805M_CTRL_DEEP_SLEEP = 0x00, // Deep Sleep
    TAS5805M_CTRL_SLEEP      = 0x01, // Sleep
    TAS5805M_CTRL_HI_Z       = 0x02, // Hi-Z
    TAS5805M_CTRL_PLAY       = 0x03, // Play
    TAS5805M_CTRL_MUTE       = 0x08  // Play
} TAS5805M_CTRL_STATE;

typedef enum {
    TAS5805M_DAC_MODE_BTL  = 0x00, // Bridge tied load
    TAS5805M_DAC_MODE_PBTL = 0x01 // Parallel load
} TAS5805M_DAC_MODE;

typedef enum {
    SW_FREQ_768K = (0x00 << 4),      
    SW_FREQ_384K = (0x01 << 4),  
    SW_FREQ_480K = (0x03 << 4), 
    SW_FREQ_576K = (0x04 << 4), 
} TAS5805M_SW_FREQ;

typedef enum {
    SW_FREQ_80K  = (0x00 << 5),
    SW_FREQ_100K = (0x01 << 5),
    SW_FREQ_120K = (0x02 << 5),
    SW_FREQ_175K = (0x03 << 5),
} TAS5805M_BD_FREQ;

typedef enum {
    MOD_MODE_BD = 0x0,
    MOD_MODE_1SPW = 0x1,
    MOD_MODE_HYBRID = 0x2,
} TAS5805M_MOD_MODE;

typedef enum {
    MIXER_UNKNOWN,
    MIXER_STEREO,
    MIXER_STEREO_INVERSE,
    MIXER_MONO,
    MIXER_RIGHT,
    MIXER_LEFT,
} TAS5805M_MIXER_MODE;


typedef enum {
    FS_FREQ_ERROR = 0b0000,
    FS_FREQ_8K    = 0b0010,
    FS_FREQ_16K   = 0b0100,
    FS_FREQ_32K   = 0b0110,
    FS_FREQ_48K   = 0b1001,
    FS_FREQ_96K   = 0b1011,
    FS_FREQ_RESERVED  = 0b1000,
} TAS5805M_FS_FREQ;


typedef struct {
    uint8_t err0;
    uint8_t err1;
    uint8_t err2;
    uint8_t ot_warn;
} TAS5805M_FAULT;

typedef struct {
    bool                     is_muted;
    bool                     is_powered;
    int8_t                   eq_gain[TAS5805M_EQ_BANDS];
    TAS5805M_CTRL_STATE      state;
    TAS5805M_MIXER_MODE      mixer_mode;
} TAS5805_STATE;

// Analog gain
#define TAS5805M_MAX_GAIN 0
#define TAS5805M_MIN_GAIN 31
static const uint8_t tas5805m_again[TAS5805M_MIN_GAIN + 1] = {
    0x00, /* 0dB */ 
    0x01, /* -0.5Db */
    0x02, /* -1.0dB */
    0x03, /* -1.5dB */
    0x04, /* -2.0dB */
    0x05, /* -2.5dB */
    0x06, /* -3.0dB */
    0x07, /* -3.5dB */
    0x08, /* -4.0dB */
    0x09, /* -4.5dB */
    0x0A, /* -5.0dB */
    0x0B, /* -5.5dB */
    0x0C, /* -6.0dB */
    0x0D, /* -6.5dB */
    0x0E, /* -7.0dB */
    0x0F, /* -7.5dB */
    0x10, /* -8.0dB */
    0x11, /* -8.5dB */
    0x12, /* -9.0dB */
    0x13, /* -9.5dB */
    0x14, /* -10.0dB */
    0x15, /* -10.5dB */
    0x16, /* -11.0dB */
    0x17, /* -11.5dB */
    0x18, /* -12.0dB */
    0x19, /* -12.5dB */
    0x1A, /* -13.0dB */
    0x1B, /* -13.5dB */
    0x1C, /* -14.0dB */
    0x1D, /* -14.5dB */
    0x1E, /* -15.0dB */
    0x1F, /* -15.5dB */
};

#define TAS5805M_VOLUME_MAX	255                 // Mute
#define TAS5805M_VOLUME_DEFAULT	48              //  +0 Db
#define TAS5805M_VOLUME_MIN	0                   // +24 Db

#define TAS5805M_VOLUME_PCT_MIN	0               // Mute
#define TAS5805M_VOLUME_PCT_DEFAULT	100         //  +0 Db
#define TAS5805M_VOLUME_PCT_MAX	124             // +24 Db


/**
     * @brief Power up and initialize TAS5805 codec chip
     *
     * @param cfg configuration of TAS5805
     *
     * @return
     *     - ESP_OK
     *     - ESP_FAIL
     */
    esp_err_t tas5805m_init();
    /**
     * @brief Deinitialize TAS5805 codec chip
     *
     * @return
     *     - ESP_OK
     *     - ESP_FAIL
     */
    esp_err_t tas5805m_deinit(void);

    /**
     * @brief  Set device volume
     *
     * @param volume: digital volume (inverted) (0~255)
     *
     * @return
     *     - ESP_OK
     *     - ESP_FAIL
     */
    esp_err_t tas5805m_set_volume(uint8_t vol);

    /**
     * @brief Get device volume
     *
     * @param[out] *volume: digital volume (inverted) (0~255)
     *
     * @return
     *     - ESP_OK
     *     - ESP_FAIL
     */
    esp_err_t tas5805m_get_volume(uint8_t *vol);
    
    /**
     * @brief  Set device volume (0..100%)
     *
     * @param volume: digital volume (inverted) (0~255)
     *
     * @return
     *     - ESP_OK
     *     - ESP_FAIL
     */
    esp_err_t tas5805m_set_volume_pct(uint8_t vol);

    /**
     * @brief Get device volume (0..100%)
     *
     * @param[out] *volume: digital volume (inverted) (0~255)
     *
     * @return
     *     - ESP_OK
     *     - ESP_FAIL
     */
    esp_err_t tas5805m_get_volume_pct(uint8_t *vol);


    /**
     * @brief Set TAS5805 mute or not
     *        Continuously call should have an interval time determined by
     * tas5805m_set_mute_fade()
     *
     * @param enable enable(1) or disable(0)
     *
     * @return
     *     - ESP_FAIL Parameter error
     *     - ESP_OK   Success
     */
    esp_err_t tas5805m_set_mute(bool enable);

    /**
     * @brief Mute TAS5805M
     *
     * @param value  Time for mute with millisecond.
     * @return
     *     - ESP_FAIL Parameter error
     *     - ESP_OK   Success
     *
     */

    /**
     * @brief Get TAS5805 mute status
     *
     *  @return
     *     - ESP_FAIL Parameter error
     *     - ESP_OK   Success
     */
    esp_err_t tas5805m_get_mute(bool *enabled);

    // do nothing
    // esp_err_t tas5805m_ctrl(audio_hal_codec_mode_t mode,
    //                         audio_hal_ctrl_t ctrl_state);

    // // do nothing
    // esp_err_t tas5805m_config_iface(audio_hal_codec_mode_t mode,
    //                                 audio_hal_codec_i2s_iface_t *iface);

    /** 
     * @brief Get the current state of the TAS5805M
     * 
     * @param state: Pointer to the state variable
     * 
    */
    esp_err_t tas5805m_get_state(TAS5805M_CTRL_STATE *state);

    /** 
     * @brief Set the state of the TAS5805M
     * 
     * @param state: The state to set
     * 
    */
    esp_err_t tas5805m_set_state(TAS5805M_CTRL_STATE state);

    /** 
     * @brief Get the current DAC mode of the TAS5805M
     * 
     * @param mode: Pointer to the mode variable
     * 
    */
    esp_err_t tas5805m_get_dac_mode(TAS5805M_DAC_MODE *mode);

    /** 
     * @brief Set the DAC mode of the TAS5805M
     * 
     * @param mode: The mode to set
     * 
    */
    esp_err_t tas5805m_set_dac_mode(TAS5805M_DAC_MODE mode);

    /** 
     * @brief Get the current EQ state of the TAS5805M
     * 
     * @param enabled: Pointer to the enabled variable
     * 
    */
    esp_err_t tas5805m_get_eq(bool *enabled);

    /** 
     * @brief Set the EQ state of the TAS5805M
     * 
     * @param enable: The state to set
     * 
    */
    esp_err_t tas5805m_set_eq(bool enable);

    /** 
     * @brief Get the current EQ gain of the TAS5805M
     * 
     * @param band: The band to get the gain of
     * @param gain: Pointer to the gain variable
     * 
    */
    esp_err_t tas5805m_get_eq_gain(int band, int *gain);

    /** 
     * @brief Set the EQ gain of the TAS5805M
     * 
     * @param band: The band to set the gain of
     * @param gain: The gain to set
     * 
    */
    esp_err_t tas5805m_set_eq_gain(int band, int gain);

    /**
     * @brief Get the current modulation mode of the TAS5805M
     * 
     * @param mode: Pointer to the mode variable
     * @param freq: Pointer to the DSP frequency variable
     * @param bd_freq: Pointer to the BD frequency variable
     * 
     * @return
     *     - ESP_OK
     *     - ESP_FAIL
     */
    esp_err_t tas5805m_get_modulation_mode(TAS5805M_MOD_MODE *mode, TAS5805M_SW_FREQ *freq, TAS5805M_BD_FREQ *bd_freq);

    /**
     * @brief Set the modulation mode of the TAS5805M
     * 
     * @param mode: The mode to set
     * @param freq: The DSP frequency to set
     * @param bd_freq: The BD frequency to set
     * 
     * @return
     *     - ESP_OK
     *     - ESP_FAIL
     */
    esp_err_t tas5805m_set_modulation_mode(TAS5805M_MOD_MODE mode, TAS5805M_SW_FREQ freq, TAS5805M_BD_FREQ bd_freq);

    /**
     * @brief Get the analog gain of the TAS5805M
     * 
     * @param gain: Pointer to the gain variable
     * 
     * @return
     *     - ESP_OK
     *     - ESP_FAIL
     */
    esp_err_t tas5805m_get_again(uint8_t *gain);

    /**
     * @brief Set the analog gain of the TAS5805M
     * 
     * @param gain: The gain to set
     * 
     * @return
     *     - ESP_OK
     *     - ESP_FAIL
     */
    esp_err_t tas5805m_set_again(uint8_t gain);

    /**
     * @brief Get the mixer mode of the TAS5805M
     * 
     * @param mode: Pointer to the mode variable
     * 
     * @return
     *     - ESP_OK
     *     - ESP_FAIL
     */
    esp_err_t tas5805m_get_mixer_mode(TAS5805M_MIXER_MODE *mode);

    /**
     * @brief Set the mixer mode of the TAS5805M
     * 
     * @param mode: The mode to set
     * 
     * @return
     *     - ESP_OK
     *     - ESP_FAIL
     */
    esp_err_t tas5805m_set_mixer_mode(TAS5805M_MIXER_MODE mode);

    /**
     * @brief Get the sample rate of the TAS5805M
     * 
     * @param freq: Pointer to the frequency variable
     * 
     * @return
     *     - ESP_OK
     *     - ESP_FAIL
     */
    esp_err_t tas5805m_get_fs_freq(TAS5805M_FS_FREQ *freq);

    /**
     * @brief Get the BCK ratio of the TAS5805M
     * 
     * @param ratio: Pointer to the ratio variable
     * 
     * @return
     *     - ESP_OK
     *     - ESP_FAIL
     */
    esp_err_t tas5805m_get_bck_ratio(uint8_t *ratio);


    /**
     * @brief Get the power state of the TAS5805M
     * 
     * @param state: Pointer to the state variable
     * 
     * @return
     *     - ESP_OK
     *     - ESP_FAIL
     */
    esp_err_t tas5805m_get_power_state(TAS5805M_CTRL_STATE *state);

    /**
     * @brief Get the automute flags of the TAS5805M
     * 
     * @param is_r_muted: Pointer to the right automute flag
     * @param is_l_muted: Pointer to the left automute flag
     * 
     * @return
     *     - ESP_OK
     *     - ESP_FAIL
     */
    esp_err_t tas5805m_get_automute_state(bool *is_r_muted, bool *is_l_muted);

    /**
     * @brief Get the faults of the TAS5805M
     * 
     * @param fault: Pointer to the fault struct
     * 
     * @return
     *     - ESP_OK
     *     - ESP_FAIL
     */
    esp_err_t tas5805m_get_faults(TAS5805M_FAULT* fault);
    
    /**
     * @brief Clear the faults of the TAS5805M
     * 
     * @return
     *     - ESP_OK
     *     - ESP_FAIL
     */
    esp_err_t tas5805m_clear_faults();

    /**
     * @brief Decode the errors from the TAS5805M
     * 
     * @param fault: The fault struct to decode
     * 
     */
    void tas5805m_decode_faults(TAS5805M_FAULT fault);

    
    /**
     * @brief Map the TAS5805M_CTRL_STATE to a string
     * 
     * @param state: The state to map
     * 
     * @return The string representation of the state
     */
    const char* tas5805m_map_amp_state(TAS5805M_CTRL_STATE state);

    /**
     * @brief Map the TAS5805M_DAC_MODE to a string
     * 
     * @param mode: The mode to map
     * 
     * @return The string representation of the mode
     */
    const char* tas5805m_map_fs_freq(TAS5805M_FS_FREQ freq);


#ifdef __cplusplus  
}
#endif

#endif