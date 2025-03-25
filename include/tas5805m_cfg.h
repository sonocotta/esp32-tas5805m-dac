#pragma once

#include "driver/i2s.h" 

#ifdef __cplusplus
extern "C"
{
#endif

/* Register Naming*/
#define I2C_MASTER_NUM I2C_NUM_0   /*!< I2C port number for master dev */
#define WRITE_BIT I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ   /*!< I2C master read */
#define ACK_CHECK_EN 0x1           /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0          /*!< I2C master will not check ack from slave */
#define ACK_VAL I2C_MASTER_ACK     /*!< I2C ack value */
#define NACK_VAL I2C_MASTER_NACK   /*!< I2C nack value */

#define I2C_TAS5805M_MASTER_NUM I2C_MASTER_NUM /*!< I2C port number for master dev */

#define TAS5805M_ADDRESS 0x2D      /*!< 7-bit address with a 15k pull up resistor */

#if CONFIG_IDF_TARGET_ESP32C3
#define GPIO_NUM_UNSET 63
#else
#define GPIO_NUM_UNSET 255
#endif

// Fault pin
#ifndef TAS5805M_GPIO_FAULT
#if CONFIG_IDF_TARGET_ESP32
#define TAS5805M_GPIO_FAULT GPIO_NUM_34
#elif CONFIG_IDF_TARGET_ESP32S3
#define TAS5805M_GPIO_FAULT GPIO_NUM_18
#else
#define TAS5805M_GPIO_FAULT GPIO_NUM_UNSET
#endif
#endif

// Power down pin
#ifndef TAS5805M_GPIO_PDN
#if CONFIG_IDF_TARGET_ESP32
#define TAS5805M_GPIO_PDN GPIO_NUM_33
#elif CONFIG_IDF_TARGET_ESP32S3
#define TAS5805M_GPIO_PDN GPIO_NUM_17
#else
#define TAS5805M_GPIO_PDN GPIO_NUM_UNSET
#endif
#endif
#define TAS5805M_GPIO_PDN_MASK ((1ULL << TAS5805M_GPIO_PDN))

// I2S interface
#ifndef TAS5805M_I2S_NUM
#define TAS5805M_I2S_NUM I2S_NUM_0
#endif

// Startup sequence codes
#define TAS5805M_CFG_META_SWITCH (255)
#define TAS5805M_CFG_META_DELAY (254)
#define TAS5805M_CFG_META_BURST (253)

#define TAS5805M_REG_PAGE_SET 0x00
#define TAS5805M_REG_BOOK_SET 0x7f

#define TAS5805M_REG_BOOK_CONTROL_PORT 0x00
#define TAS5805M_REG_PAGE_ZERO 0x00

#define TAS5805M_RESET_CTRL 0x01
#define TAS5805M_DEVICE_CTRL_1 0x02
#define TAS5805M_DEVICE_CTRL_2 0x03
#define TAS5805M_I2C_PAGE_AUTO_INC 0x0f
#define TAS5805M_SIG_CH_CTRL 0x28
#define TAS5805M_CLOCK_DET_CTRL 0x29
#define TAS5805M_SDOUT_SEL 0x30
#define TAS5805M_I2S_CTRL 0x31
#define TAS5805M_SAP_CTRL1 0x33
#define TAS5805M_SAP_CTRL2 0x34
#define TAS5805M_SAP_CTRL3 0x35
#define TAS5805M_FS_MON 0x37
#define TAS5805M_BCK_MON 0x38
#define TAS5805M_CLKDET_STATUS 0x39
#define TAS5805M_CHANNEL_FORCE_HIZ 0x40
#define TAS5805M_DIG_VOL_CTRL 0x4c
#define TAS5805M_DIG_VOL_CTRL2 0x4e
#define TAS5805M_DIG_VOL_CTRL3 0x4f
#define TAS5805M_AUTO_MUTE_CTRL 0x50
#define TAS5805M_AUTO_MUTE_TIME 0x51
#define TAS5805M_ANA_CTRL 0x53
#define TAS5805M_AGAIN 0x54
#define TAS5805M_BQ_WR_CTRL1 0x5c
#define TAS5805M_DAC_CTRL 0x5d
#define TAS5805M_ADR_PIN_CTRL 0x60
#define TAS5805M_ADR_PIN_CONFIG 0x61
#define TAS5805M_DSP_MISC 0x66
#define TAS5805M_DIE_ID 0x67
#define TAS5805M_POWER_STATE 0x68
#define TAS5805M_AUTOMUTE_STATE 0x69
#define TAS5805M_PHASE_CTRL 0x6a
#define TAS5805M_SS_CTRL0 0x6b
#define TAS5805M_SS_CTRL1 0x6c
#define TAS5805M_SS_CTRL2 0x6d
#define TAS5805M_SS_CTRL3 0x6e
#define TAS5805M_SS_CTRL4 0x6f
#define TAS5805M_CHAN_FAULT 0x70
#define TAS5805M_GLOBAL_FAULT1 0x71
#define TAS5805M_GLOBAL_FAULT2 0x72
#define TAS5805M_OT_WARNING 0x73
#define TAS5805M_PIN_CONTROL1 0x74
#define TAS5805M_PIN_CONTROL2 0x75
#define TAS5805M_MISC_CONTROL 0x76
#define TAS5805M_FAULT_CLEAR 0x78
/* TAS5805M_REG_FAULT register values */
#define TAS5805M_ANALOG_FAULT_CLEAR 0x80

// EQ registers
#define TAS5805M_CTRL_EQ_ON 0x00
#define TAS5805M_CTRL_EQ_OFF 0x01

// Level meter register
#define TAS5805M_REG_BOOK_4 0x78
#define TAS5805M_REG_BOOK_4_LEVEL_METER_PAGE 0x02
#define TAS5805M_REG_LEVEL_METER_LEFT 0x60
#define TAS5805M_REG_LEVEL_METER_RIGHT 0x64

// Mixer registers
#define TAS5805M_REG_BOOK_5 0x8c
#define TAS5805M_REG_BOOK_5_MIXER_PAGE 0x29
#define TAS5805M_REG_LEFT_TO_LEFT_GAIN 0x18
#define TAS5805M_REG_RIGHT_TO_LEFT_GAIN 0x1c
#define TAS5805M_REG_LEFT_TO_RIGHT_GAIN 0x20
#define TAS5805M_REG_RIGHT_TO_RIGHT_GAIN 0x24
#define TAS5805M_REG_BOOK_5_VOLUME_PAGE 0x2a
#define TAS5805M_REG_LEFT_VOLUME 0x24
#define TAS5805M_REG_RIGHT_VOLUME 0x28
#define TAS5805M_MIXER_VALUE_MUTE 0x00
#define TAS5805M_MIXER_VALUE_0DB 0x00008000
#define TAS5805M_MIXER_VALUE_MINUS6DB 0xE7264000  

#ifdef __cplusplus
}
#endif