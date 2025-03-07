#pragma once

// #include "audio_hal.h"

#include "esp_err.h"
#include "esp_log.h"
// #include "board.h"

#include "../eq/tas5805m_eq.h"

#ifdef __cplusplus
extern "C"
{
#endif

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
    SW_FREQ_768K = 0x00,      
    SW_FREQ_384K = 0x01,  
    SW_FREQ_480K = 0x03, 
    SW_FREQ_576K = 0x04, 
} TAS5805M_SW_FREQ;

typedef enum {
    SW_FREQ_80K = 0x00,
    SW_FREQ_100K = 0x01,
    SW_FREQ_120K = 0x02,
    SW_FREQ_175K = 0x03,
} TAS5805M_BD_FREQ;

typedef enum {
    MOD_MODE_BD = (0x0 << 4),
    MOD_MODE_1SPW = (0x1 << 4),
    MOD_MODE_HYBRID = (0x2 << 4),
} TAS5805M_MOD_MODE;

typedef enum {
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
    bool                     eq_is_on;        
    int				         volume;
    int8_t                   eq_gain[TAS5805M_EQ_BANDS];
    uint8_t                  a_gain;
    TAS5805M_CTRL_STATE      state;
    TAS5805M_DAC_MODE        dac_mode;
    TAS5805M_SW_FREQ         sw_freq;
    TAS5805M_BD_FREQ         bd_freq;
    TAS5805M_MOD_MODE        mod_mode;
    TAS5805M_MIXER_MODE      mixer_mode;
} TAS5805_STATE;

// Analog gain
#define TAS5805M_MIN_GAIN 0
#define TAS5805M_MAX_GAIN 31
static const uint8_t tas5805m_again[TAS5805M_MAX_GAIN + 1] = {
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

// Digital volume
static const uint32_t tas5805m_volume[] = {
    0x1B000000, /*   0, -110dB */ 0x1E000000, /*   1, -109dB */
	0x21000000, /*   2, -108dB */ 0x25000000, /*   3, -107dB */
	0x2A000000, /*   4, -106dB */ 0x2F000000, /*   5, -105dB */
	0x35000000, /*   6, -104dB */ 0x3B000000, /*   7, -103dB */
	0x43000000, /*   8, -102dB */ 0x4B000000, /*   9, -101dB */
	0x54000000, /*  10, -100dB */ 0x5E000000, /*  11,  -99dB */
	0x6A000000, /*  12,  -98dB */ 0x76000000, /*  13,  -97dB */
	0x85000000, /*  14,  -96dB */ 0x95000000, /*  15,  -95dB */
	0xA7000000, /*  16,  -94dB */ 0xBC000000, /*  17,  -93dB */
	0xD3000000, /*  18,  -92dB */ 0xEC000000, /*  19,  -91dB */
	0x09010000, /*  20,  -90dB */ 0x2A010000, /*  21,  -89dB */
	0x4E010000, /*  22,  -88dB */ 0x77010000, /*  23,  -87dB */
	0xA4010000, /*  24,  -86dB */ 0xD8010000, /*  25,  -85dB */
	0x11020000, /*  26,  -84dB */ 0x52020000, /*  27,  -83dB */
	0x9A020000, /*  28,  -82dB */ 0xEC020000, /*  29,  -81dB */
	0x47030000, /*  30,  -80dB */ 0xAD030000, /*  31,  -79dB */
	0x20040000, /*  32,  -78dB */ 0xA1040000, /*  33,  -77dB */
	0x32050000, /*  34,  -76dB */ 0xD4050000, /*  35,  -75dB */
	0x8A060000, /*  36,  -74dB */ 0x56070000, /*  37,  -73dB */
	0x3B080000, /*  38,  -72dB */ 0x3C090000, /*  39,  -71dB */
	0x5D0A0000, /*  40,  -70dB */ 0xA00B0000, /*  41,  -69dB */
	0x0C0D0000, /*  42,  -68dB */ 0xA30E0000, /*  43,  -67dB */
	0x6C100000, /*  44,  -66dB */ 0x6D120000, /*  45,  -65dB */
	0xAD140000, /*  46,  -64dB */ 0x33170000, /*  47,  -63dB */
	0x071A0000, /*  48,  -62dB */ 0x341D0000, /*  49,  -61dB */
	0xC5200000, /*  50,  -60dB */ 0xC4240000, /*  51,  -59dB */
	0x41290000, /*  52,  -58dB */ 0x492E0000, /*  53,  -57dB */
	0xEF330000, /*  54,  -56dB */ 0x453A0000, /*  55,  -55dB */
	0x61410000, /*  56,  -54dB */ 0x5C490000, /*  57,  -53dB */
	0x4F520000, /*  58,  -52dB */ 0x5A5C0000, /*  59,  -51dB */
	0x9F670000, /*  60,  -50dB */ 0x44740000, /*  61,  -49dB */
	0x74820000, /*  62,  -48dB */ 0x5F920000, /*  63,  -47dB */
	0x3BA40000, /*  64,  -46dB */ 0x45B80000, /*  65,  -45dB */
	0xC1CE0000, /*  66,  -44dB */ 0xFBE70000, /*  67,  -43dB */
	0x49040000, /*  68,  -42dB */ 0x0C240000, /*  69,  -41dB */
	0xAE470000, /*  70,  -40dB */ 0xAA6F0000, /*  71,  -39dB */
	0x869C0000, /*  72,  -38dB */ 0xDCCE0000, /*  73,  -37dB */
	0x56200000, /*  74,  -36dB */ 0xB5460000, /*  75,  -35dB */
	0xCF8D0000, /*  76,  -34dB */ 0x96DD0000, /*  77,  -33dB */
	0x18370000, /*  78,  -32dB */ 0x879B0000, /*  79,  -31dB */
	0x374C0000, /*  80,  -30dB */ 0xA78A0000, /*  81,  -29dB */
	0x84510000, /*  82,  -28dB */ 0xB7B50000, /*  83,  -27dB */
	0x4A6A0000, /*  84,  -26dB */ 0xAE730000, /*  85,  -25dB */
	0x85130000, /*  86,  -24dB */ 0xCC0F0000, /*  87,  -23dB */
	0xDB2A0000, /*  88,  -22dB */ 0x73680000, /*  89,  -21dB */
    0xCDCC0C00, /*  90,  -20dB */ 0xA15C0E00, /*  91,  -19dB */
	0x3F1D1000, /*  92,  -18dB */ 0x9A141200, /*  93,  -17dB */
	0x61491400, /*  94,  -16dB */ 0x11C31600, /*  95,  -15dB */
	0x138A1900, /*  96,  -14dB */ 0xD7A71C00, /*  97,  -13dB */
	0xF3262000, /*  98,  -12dB */ 0x47132400, /*  99,  -11dB */
	0x277A2800, /* 100,  -10dB */ 0x866A2D00, /* 101,  -9dB */
	0x2D5F3200, /* 102,   -8dB */ 0xEE2C3900, /* 103,   -7dB */
	0xE7264000, /* 104,   -6dB */ 0xCDFA4700, /* 105,   -5dB */
	0x36C35000, /* 106,   -4dB */ 0xF89D5A00, /* 107,   -3dB */
	0x8CAC6500, /* 108,   -2dB */ 0x83147200, /* 109,   -1dB */
	0x00008000, /* 110,    0dB */ 0x4D9E8F00, /* 111,    1dB */
	0x7824A100, /* 112,    2dB */ 0x08CEB400, /* 113,    3dB */
	0xC8DDCA00, /* 114,    4dB */ 0x9A9EE300, /* 115,    5dB */
	0xC164FF00, /* 116,    6dB */ 0x6A8E1E01, /* 117,    7dB */
	0x7F854101, /* 118,    8dB */ 0xC6C06801, /* 119,    9dB */
	0x84C59401, /* 120,   10dB */ 0x4029C601, /* 121,   11dB */
	0xC293FD01, /* 122,   12dB */ 0x48C13B02, /* 123,   13dB */
	0x08858102, /* 124,   14dB */ 0x01CCCF02, /* 125,   15dB */
	0x1AA02703, /* 126,   16dB */ 0xAD2B8A03, /* 127,   17dB */
	0x7ABD8B03, /* 128,   18dB */ 0x1BCD7404, /* 129,   19dB */
	0x00000005, /* 130,   20dB */ 0x022F9C05, /* 131,   21dB */
	0xAE6C4B06, /* 132,   22dB */ 0x4D0C1007, /* 133,   23dB */
	0xCDA9EC07, /* 134,   24dB */ 0x9932E408, /* 135,   25dB */
	0x8EEF9F09, /* 136,   26dB */ 0x2590310B, /* 137,   27dB */
	0xF2368F0C, /* 138,   28dB */ 0xB887170E, /* 139,   29dB */
	0x25B7CF0F, /* 140,   30dB */ 0x849CBD11, /* 141,   31dB */
	0x94C5E713, /* 142,   32dB */ 0xCB8C5516, /* 143,   33dB */
	0x54320F19, /* 144,   34dB */ 0x0EF81D1C, /* 145,   35dB */
	0x07418C1F, /* 146,   36dB */ 0xBFB46523, /* 147,   37dB */
	0xC266B727, /* 148,   38dB */ 0x1303902C, /* 149,   39dB */
	0x00000032, /* 150,   40dB */ 0x12D61938, /* 151,   41dB */
	0xCA3EF23E, /* 152,   42dB */ 0x077BA046, /* 153,   43dB */
	0x03A23E4F, /* 154,   44dB */ 0xF9F9E958, /* 155,   45dB */
	0x8E5BC363, /* 156,   46dB */ 0x6DA1EF6F, /* 157,   47dB */
	0x7525987D, /* 158,   48dB */
};

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define TAS5805M_VOLUME_MAX	((int)ARRAY_SIZE(tas5805m_volume) - 1)
#define TAS5805M_VOLUME_MIN	0
#define TAS5805M_VOLUME_DEFAULT	110

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
     * @brief  Set voice volume
     *
     * @param volume:  voice volume (0~100)
     *
     * @return
     *     - ESP_OK
     *     - ESP_FAIL
     */
    esp_err_t tas5805m_set_volume(int vol);

    /**
     * @brief Get voice volume
     *
     * @param[out] *volume:  voice volume (0~100)
     *
     * @return
     *     - ESP_OK
     *     - ESP_FAIL
     */
    esp_err_t tas5805m_get_volume(int *vol);

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
    esp_err_t tas5805m_get_again(int *gain);

    /**
     * @brief Set the analog gain of the TAS5805M
     * 
     * @param gain: The gain to set
     * 
     * @return
     *     - ESP_OK
     *     - ESP_FAIL
     */
    esp_err_t tas5805m_set_again(int gain);

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

#ifdef __cplusplus  
}
#endif
