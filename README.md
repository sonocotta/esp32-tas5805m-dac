# ESP32 TAS5805M DAC Library

This library provides an interface for controlling the TAS5805M digital-to-analog converter (DAC) on the ESP32 platform. The library allows you to initialize the DAC, set and get various parameters such as volume, mute state, DAC mode, EQ settings, and more.

## Features

- Initialize and deinitialize the TAS5805M DAC
- Control DAC
  - Set and get volume
  - Set and get mute state
  - Set and get DAC mode
  - Set and get EQ state and gain
  - Set and get modulation mode
  - Set and get analog gain
  - Set and get mixer mode
- Read DAC state
  - Get sample rate and BCK ratio
  - Get power state and auto-mute state
  - Get and clear fault states
  - Decode fault errors

## Installation

1. Clone the repository:
    ```sh
    git clone https://github.com/yourusername/esp32-tas5805m-dac.git
    ```

2. Include the library in your project:
    ```cpp
    #include "tas5805m.h"
    ```

## Usage

### Important notes

The `tas5805m.h` is a low-level driver with minimum dependencies (it uses only esp-idf). Lack of dependencies comes with a small price: few things need to be initialized prior to driver initialization, or it will fail. More specifically, the driver will expect
- I2C communication initialized and ready. It will use esp-idf communication functions, assuming I2C is ready.
- I2S peripheral is initialized and ready. DAC requires seeing an I2S clock for a few milliseconds, or it will ignore many of the DSP settings.

I've implemented a `tas5805m.hpp` wrapper, that I'm primarily using in the Arduino code, so it becomes pretty easy:

```cpp
#include <tas5805m.hpp>
tas5805m Tas5805m(&Wire);
```

```cpp
const int sampleRate = 16000;
const int bps = 16;
I2S.begin(I2S_PHILIPS_MODE, sampleRate, bps);
Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
Tas5805m.init();

```

Next, the esp-adf component implementation was created specifically for [snapclient](https://github.com/sonocotta/esparagus-snapclient/) implementation, but it should work with other esp-adf code as well

With that disclaimer, here is what it does.

### Initialization

To initialize the TAS5805M DAC, call the `tas5805m_init` function:

```cpp
#include "tas5805m.h"

void app_main() {
    esp_err_t ret = tas5805m_init();
    if (ret != ESP_OK) {
        ESP_LOGE("TAS5805M", "Failed to initialize TAS5805M");
    }
}
```

## Digital Volume and Analog Gain

> A combination of digital gain and analog gain is used to provide the overall gain of the speaker amplifier. The total amplifier gain consists of the digital gain and the analog gain from the input of the analog modulator to the output of the speaker amplifier power stage.

> The first gain stage of the speaker amplifier is present in the digital audio path. Digital
gain consists of the volume control, input Mixer, or output Crossbar. The digital gain is set to 0dB by default.
Change analog gain via register 0x54, AGAIN[4:0], which supports 32 32-step analog gain setting (0.5dB per step).
These analog gain settings ensure that the output signal is not clipped at different PVDD levels. 0dBFS output
corresponds to 29.5-V peak output voltage. 

<details>
<summary>Analog gain</summary>

| Binary | dB Value | Output Voltage (V) |
|--------|----------|--------------------|
| 0      | 0        | 29.5               |
| 1      | -0.5     | 27.92              |
| 10     | -1       | 26.42              |
| 11     | -1.5     | 25                 |
| 100    | -2       | 23.65              |
| 101    | -2.5     | 22.38              |
| 110    | -3       | 21.17              |
| 111    | -3.5     | 20.02              |
| 1000   | -4       | 18.94              |
| 1001   | -4.5     | 17.91              |
| 1010   | -5       | 16.94              |
| 1011   | -5.5     | 16.02              |
| 1100   | -6       | 15.15              |
| 1101   | -6.5     | 14.33              |
| 1110   | -7       | 13.55              |
| 1111   | -7.5     | 12.82              |
| 10000  | -8       | 12.13              |
| 10001  | -8.5     | 11.48              |
| 10010  | -9       | 10.87              |
| 10011  | -9.5     | 10.29              |
| 10100  | -10      | 9.75               |
| 10101  | -10.5    | 9.24               |
| 10110  | -11      | 8.76               |
| 10111  | -11.5    | 8.3                |
| 11000  | -12      | 7.87               |
| 11001  | -12.5    | 7.46               |
| 11010  | -13      | 7.08               |
| 11011  | -13.5    | 6.71               |
| 11100  | -14      | 6.37               |
| 11101  | -14.5    | 6.04               |
| 11110  | -15      | 5.73               |
| 11111  | -15.5    | 4.95               |

</details>

Having the analog gain set at the appropriate level, the digital volume should be used to set the desired audio volume. Keep in mind, it is **perfectly safe to set the analog gain at a lower level**, further avoiding clipping (and effectively limiting output power) and reducing digital distortions caused by low digital gain. 

### Setting and Getting Volume

There are two ways volume can be changed. The traditional way is to use a 0-100% range, where 0 means mute, 100% means 0 dB gain or full output swing (rail-to-rail voltage). In the case of TAS5805M DAc, it allows for gains up to +24 dB, which might be used with lower amplitude audio, but will cause distortions normally. That's why the function would accept values up to 124, but keep in mind that only values up to 100 are guaranteed not to cause clipping.

To set the volume, use the `tas5805m_set_volume_pct` function (volume is in the range [TAS5805M_VOLUME_PCT_MIN..TAS5805M_VOLUME_PCT_MAX], default is `TAS5805M_VOLUME_PCT_DEFAULT`

```cpp
uint8_t volume = 75; // Volume level (0-124)
esp_err_t ret = tas5805m_set_volume_pct(volume);
if (ret != ESP_OK) {
    ESP_LOGE("TAS5805M", "Failed to set volume");
}
```

To get the current volume, use the `tas5805m_get_volume_pct` function:

```cpp
uint8_t volume;
esp_err_t ret = tas5805m_get_volume_pct(&volume);
if (ret != ESP_OK) {
    ESP_LOGE("TAS5805M", "Failed to get volume");
} else {
    ESP_LOGI("TAS5805M", "Current volume: %d", volume);
}
```

The second way to change the volume is to use DAC native scale, which is [0..255] where 0 is +24 dB gain (that's loud!), and 255 is mute. To set the volume this way, use the `tas5805m_set_volume` function (volume is in the range [TAS5805M_VOLUME_MIN..TAS5805M_VOLUME_MAX], default is `TAS5805M_VOLUME_DEFAULT = 48`, which is +0 Db):

```cpp
uint8_t volume = 80; // Volume level (0-255)
esp_err_t ret = tas5805m_set_volume(volume);
if (ret != ESP_OK) {
    ESP_LOGE("TAS5805M", "Failed to set volume");
}
```

To get the current volume, use the `tas5805m_get_volume` function:

```cpp
uint8_t volume;
esp_err_t ret = tas5805m_get_volume(&volume);
if (ret != ESP_OK) {
    ESP_LOGE("TAS5805M", "Failed to get volume");
} else {
    ESP_LOGI("TAS5805M", "Current volume: %d", volume);
}
```
### Setting and Getting Analog Gain

To set the analog gain, use the `tas5805m_set_again` function (value in the inverted scale [TAS5805M_MAX_GAIN..TAS5805M_MIN_GAIN], which is [0..31], representing 0.5 dB steps from 0 dB to -15.5 dB, default is 0):

```cpp
uint8_t gain = 10; // Gain level of -5 Db
esp_err_t ret = tas5805m_set_again(gain);
if (ret != ESP_OK) {
    ESP_LOGE("TAS5805M", "Failed to set analog gain");
}
```

To get the current analog gain, use the `tas5805m_get_again` function:

```cpp
uint8_t gain;
esp_err_t ret = tas5805m_get_again(&gain);
if (ret != ESP_OK) {
    ESP_LOGE("TAS5805M", "Failed to get analog gain");
} else {
    ESP_LOGI("TAS5805M", "Current analog gain: %d", gain);
}
```

## Power states

Power state is one of the following states:

| Audio state (recommended) | Power State                             | DAC Power consumption                       |
| ---------------- | ------------------------------------------------ | ------------------------------------------- |
| **Play**         | Set **TAS5805M_CTRL_PLAY** state (clear `MUTE` and `PDN`)   | ≈ 0.5W to 5W+ (depends on output power)<br> |
| **Pause**        | Set **TAS5805M_CTRL_MUTE** (keep power but silence output)     | ≈ 0.5W to 1W<br>                            |
| **Stop**         | Set **TAS5805M_CTRL_HI_Z** (disable output but stay powered) | ≈ 0.3W to 0.8W<br>                          |
| **Idle/Timeout** | Set **TAS5805M_CTRL_SLEEP** (lowest power mode)                | ≈ 0.05W to 0.1W<br>                         |
| **Unused**       | Set **TAS5805M_CTRL_DEEP_SLEEP** to ensure long life on batteries while unused | Didn't measure, should be lower than TAS5805M_CTRL_SLEEP |

The default is PLAY state, and it will keep in PLAY forever unless specifically told to

### Setting and Getting Power State

To set the power state, use the `tas5805m_set_state` function:

```cpp
TAS5805M_CTRL_STATE state = TAS5805M_CTRL_SLEEP;
esp_err_t ret = tas5805m_set_state(state);
if (ret != ESP_OK) {
    ESP_LOGE("TAS5805M", "Failed to set power state");
}
```

To get the current power state, use the `tas5805m_get_state` function:

```cpp
TAS5805M_CTRL_STATE state;
esp_err_t ret = tas5805m_get_state(&state);
if (ret != ESP_OK) {
    ESP_LOGE("TAS5805M", "Failed to get power state");
} else {
    ESP_LOGI("TAS5805M", "Current power state: %s", tas5805m_map_amp_state(state));
}
```

### Setting and Getting Mute State 

Mute is a special case of a power state, but since it will be typically used to shut down the amp when idling, I exposed it 

To set the mute state, use the `tas5805m_set_mute` function:

```cpp
bool mute = true; // Mute the DAC
esp_err_t ret = tas5805m_set_mute(mute);
if (ret != ESP_OK) {
    ESP_LOGE("TAS5805M", "Failed to set mute state");
}
```

To get the current mute state, use the `tas5805m_get_mute` function:

```cpp
bool mute;
esp_err_t ret = tas5805m_get_mute(&mute);
if (ret != ESP_OK) {
    ESP_LOGE("TAS5805M", "Failed to get mute state");
} else {
    ESP_LOGI("TAS5805M", "Mute state: %s", mute ? "Muted" : "Unmuted");
}
```

### Setting and Getting DAC Mode

TAS5805M has a bridge mode of operation, that causes both output drivers to synchronize and push out the same audio with double the power.  In that case single speaker is expected to be connected across channels, so remember to reconnect speakers if you're changing to bridge mode. 

|   | BTL (default, STEREO) | PBTL (MONO, rougly double power) |
|---|-----------------------|---------------------------|
| Descriotion | Bridge Tied Load, Stereo | Parallel Bridge Tied Load, Mono |
| Rated Power | 2×23W (8-Ω, 21 V, THD+N=1%) | 45W (4-Ω, 21 V, THD+N=1%) |
| Schematics | ![image](https://github.com/sonocotta/esp32-audio-dock/assets/5459747/e7ada8c0-c906-4c08-ae99-be9dfe907574) | ![image](https://github.com/sonocotta/esp32-audio-dock/assets/5459747/55f5315a-03eb-47c8-9aea-51e3eb3757fe)
| Speaker Connection | ![image](https://github.com/user-attachments/assets/8e5e9c38-2696-419b-9c5b-d278c655b0db) | ![image](https://github.com/user-attachments/assets/8aba6273-84c4-45a8-9808-93317d794a44)


To set the DAC mode, use the `tas5805m_set_dac_mode` function:

```cpp
TAS5805M_DAC_MODE mode = TAS5805M_DAC_MODE_BTL; // Set to Bridge Tied Load mode
esp_err_t ret = tas5805m_set_dac_mode(mode);
if (ret != ESP_OK) {
    ESP_LOGE("TAS5805M", "Failed to set DAC mode");
}
```

To get the current DAC mode, use the `tas5805m_get_dac_mode` function:

```cpp
TAS5805M_DAC_MODE mode;
esp_err_t ret = tas5805m_get_dac_mode(&mode);
if (ret != ESP_OK) {
    ESP_LOGE("TAS5805M", "Failed to get DAC mode");
} else {
    ESP_LOGI("TAS5805M", "Current DAC mode: %d", mode);
}
```

## EQ controls

TAS5805M DAC has a powerful 15-channel EQ that allows defining each channel's transfer function using BQ coefficients. In a practical sense, it allows us to draw pretty much any curve in a frequency response. I decided to split the audio range into 15 sections, defining for each a -15..+15 dB adjustment range and appropriate bandwidth to cause mild overlap. This allows both to keep the curve flat enough to not cause distortions even in extreme settings, but also allows a wide range of transfer characteristics. This EQ setup is a common approach for full-range speakers; the subwoofer-specific setup is underway.

| Band | Center Frequency (Hz) | Frequency Range (Hz) | Q-Factor (Approx.) |
|------|-----------------------|----------------------|--------------------|
| 1    | 20                    | 10–30                | 2                  |
| 2    | 31.5                  | 20–45                | 2                  |
| 3    | 50                    | 35–70                | 1.5                |
| 4    | 80                    | 55–110               | 1.5                |
| 5    | 125                   | 85–175               | 1                  |
| 6    | 200                   | 140–280              | 1                  |
| 7    | 315                   | 220–440              | 0.9                |
| 8    | 500                   | 350–700              | 0.9                |
| 9    | 800                   | 560–1120             | 0.8                |
| 10   | 1250                  | 875–1750             | 0.8                |
| 11   | 2000                  | 1400–2800            | 0.7                |
| 12   | 3150                  | 2200–4400            | 0.7                |
| 13   | 5000                  | 3500–7000            | 0.6                |
| 14   | 8000                  | 5600–11200           | 0.6                |
| 15   | 16000                 | 11200–20000          | 0.5                |

Here are a few examples of different configurations that can be created with the above setup. 

  ![image](https://github.com/user-attachments/assets/91f360fa-7e2a-4ca8-8b72-4ed7830bf7f7)
  ![image](https://github.com/user-attachments/assets/15164675-8899-44b7-a551-0585a2a7fd8c)
  ![image](https://github.com/user-attachments/assets/31f17a19-9dbe-4e9e-947e-0cbcccbf218c)
  ![image](https://github.com/user-attachments/assets/c0445bd6-29f6-44d0-9632-14becc1de35e)


### Setting and Getting EQ State and Gain

To set the EQ state, use the `tas5805m_set_eq` function:

```cpp
bool eq_enabled = true; // Enable EQ
esp_err_t ret = tas5805m_set_eq(eq_enabled);
if (ret != ESP_OK) {
    ESP_LOGE("TAS5805M", "Failed to set EQ state");
}
```

To get the current EQ state, use the `tas5805m_get_eq` function:

```cpp
bool eq_enabled;
esp_err_t ret = tas5805m_get_eq(&eq_enabled);
if (ret != ESP_OK) {
    ESP_LOGE("TAS5805M", "Failed to get EQ state");
} else {
    ESP_LOGI("TAS5805M", "EQ state: %s", eq_enabled ? "Enabled" : "Disabled");
}
```

To set the EQ gain, use the `tas5805m_set_eq_gain` function:

```cpp
int band = 1; // EQ band
int gain = 5; // Gain level
esp_err_t ret = tas5805m_set_eq_gain(band, gain);
if (ret != ESP_OK) {
    ESP_LOGE("TAS5805M", "Failed to set EQ gain");
}
```

To get the current EQ gain, use the `tas5805m_get_eq_gain` function:

```cpp
int band = 1; // EQ band
int gain;
esp_err_t ret = tas5805m_get_eq_gain(band, &gain);
if (ret != ESP_OK) {
    ESP_LOGE("TAS5805M", "Failed to get EQ gain");
} else {
    ESP_LOGI("TAS5805M", "EQ gain for band %d: %d", band, gain);
}
```

## Modulation modes and switching frequency

Both modulation scheme and switching frequency have an impact on power consumption / switching losses versus EMI noise. 

#### BD Modulation

> This is a modulation scheme that allows operation without the classic LC reconstruction filter when the amp is
driving an inductive load with short speaker wires. Each output is switching from 0 volts to the supply voltage.
The OUTPx and OUTNx are in phase with each other, with no input, so that there is little or no current in the
speaker. The duty cycle of OUTPx is greater than 50% and OUTNx is less than 50% for positive output voltages.
The duty cycle of OUTPx is less than 50%, and OUTNx is greater than 50% for negative output voltages. The
voltage across the load sits at 0 V throughout most of the switching period, reducing the switching current, which
reduces any I2R losses in the load.

#### 1SPW Modulation

> The 1SPW mode alters the normal modulation scheme in order to achieve higher efficiency with a slight penalty
in THD degradation and more attention required in the output filter selection. In Low Idle Current mode, the
outputs operate at ~14% modulation during idle conditions. When an audio signal is applied, one output will
decrease and one will increase. The decreasing output signal will quickly rail to GND, at which point all the audio
modulation takes place through the rising output. The result is that only one output is switching during a majority
of the audio cycle. Efficiency is improved in this mode due to the reduction of switching losses.

#### Hybrid Modulation

> Hybrid Modulation is designed to minimize power loss without compromising the THD+N performance, and is
optimized for battery-powered applications. With Hybrid modulation enabled, the device detects the input signal level
and adjusts the PWM duty cycle dynamically based on PVDD. Hybrid modulation achieves ultra-low idle current and
maintains the same audio performance level as the BD Modulation. In order to minimize the power dissipation,
low switching frequency (For example, Fsw = 384 kHz) with a proper LC filter (15 µH + 0.68 µF or 22 µH + 0.68
µF) is recommended

#### Driver Switching frequency

TAS5805M supports different switching frequencies, which mostly affect the balance between output filter losses and EMI noise. Below is the recommendation from TI

![image](https://github.com/user-attachments/assets/72d7c8cf-1e47-4b92-b191-c7f4a6728bd0)

- Ferrite bead filter is appropriate for lower PVCC (< 12V)
- Ferrite bead filter is recommended for use with  Fsw = 384 kHz with Spread spectrum enabled, BD Modulation
- With an inductor as the output filter, DAC can achieve ultra-low idle current (with Hybrid Modulation or 1SPW Modulation) and keep a large EMI margin. The switching frequency of TAS5805M can be adjusted from 384 kHz to 768 kHz. Higher switching frequency means a smaller Inductor value needed
  - With 768 kHz switching frequency. Designers can select 10uH + 0.68 µF or 4.7 µH +0.68 µF as the output filter, which will help customers to save the Inductor size with the same rated current during the inductor selection. With 4.7uH + 0.68uF, make sure PVDD ≤ 12V to avoid the large ripple current to trigger the OC threshold (5A)
  - With 384 kHz switching frequency. Designers can select 22 µH + 0.68 µF or 15 µH + 0.68 µF or 10 µH + 0.68 µF as the output filter, this will help customers to save power dissipation for some battery power supply applications. With 10 µH + 0.68 µF, make sure PVDD ≤ 12 V to avoid the large ripple current from triggering the OC threshold (5 A).

### Setting and Getting Modulation Mode

To set the modulation mode, use the `tas5805m_set_modulation_mode` function:

```cpp
TAS5805M_MOD_MODE mode = MOD_MODE_BD;
TAS5805M_SW_FREQ freq = SW_FREQ_768K;
TAS5805M_BD_FREQ bd_freq = SW_FREQ_80K;
esp_err_t ret = tas5805m_set_modulation_mode(mode, freq, bd_freq);
if (ret != ESP_OK) {
    ESP_LOGE("TAS5805M", "Failed to set modulation mode");
}
```

To get the current modulation mode, use the `tas5805m_get_modulation_mode` function:

```cpp
TAS5805M_MOD_MODE mode;
TAS5805M_SW_FREQ freq;
TAS5805M_BD_FREQ bd_freq;
esp_err_t ret = tas5805m_get_modulation_mode(&mode, &freq, &bd_freq);
if (ret != ESP_OK) {
    ESP_LOGE("TAS5805M", "Failed to get modulation mode");
} else {
    ESP_LOGI("TAS5805M", "Modulation mode: %d, SW freq: %d, BD freq: %d", mode, freq, bd_freq);
}
```

## Mixer controls

Mixer settings allow to mix channel signals and route them to the appropriate channel. The typical setup for the mixer is to send Left channel audio to the Left driver, and Right channel to the Right 


![image](https://github.com/user-attachments/assets/d1a24adf-a417-48a1-b35d-39ee9d199587)

A common alternative is to combine both channels into true Mono (you need to reduce both to -3Db to compensate for signal doubling)

![image](https://github.com/user-attachments/assets/390d1ecb-e3cd-4fff-8951-80fc318ec7d9)

Of course, you can decide to use a single channel or a mix of two, just keep in mind that the sum of the signal may cause clipping if not compensated properly.

### Setting and Getting Mixer Mode

To set the mixer mode, use the `tas5805m_set_mixer_mode` function:

```cpp
TAS5805M_MIXER_MODE mode = MIXER_STEREO;
esp_err_t ret = tas5805m_set_mixer_mode(mode);
if (ret != ESP_OK) {
    ESP_LOGE("TAS5805M", "Failed to set mixer mode");
}
```

To get the current mixer mode, use the `tas5805m_get_mixer_mode` function:

```cpp
TAS5805M_MIXER_MODE mode;
esp_err_t ret = tas5805m_get_mixer_mode(&mode);
if (ret != ESP_OK) {
    ESP_LOGE("TAS5805M", "Failed to get mixer mode");
} else {
    ESP_LOGI("TAS5805M", "Current mixer mode: %d", mode);
}
```

## DAC state functions

### Getting Sample Rate and BCK Ratio

The DAC should auto-detect the sample rate and BCK clock frequency. I added these functions purely for debugging issues (in case you switch them for any reason).

To get the current sample rate, use the `tas5805m_get_fs_freq` function:

```cpp
TAS5805M_FS_FREQ freq;
esp_err_t ret = tas5805m_get_fs_freq(&freq);
if (ret != ESP_OK) {
    ESP_LOGE("TAS5805M", "Failed to get sample rate");
} else {
    ESP_LOGI("TAS5805M", "Sample rate: %d", freq);
}
```

To get the BCK ratio, use the `tas5805m_get_bck_ratio` function:

```cpp
uint8_t ratio;
esp_err_t ret = tas5805m_get_bck_ratio(&ratio);
if (ret != ESP_OK) {
    ESP_LOGE("TAS5805M", "Failed to get BCK ratio");
} else {
    ESP_LOGI("TAS5805M", "BCK ratio: %d", ratio);
}
```

### Getting Power State and Automute State

Power state pretty much represents the value from the `tas5805m_get_state` function, but it is read directly from the DAC register, so it would be a more precise state (affected by power glitches, auto-shutdown, fault, or any other reason)

To get the power state, use the `tas5805m_get_power_state` function:

```cpp
TAS5805M_CTRL_STATE state;
esp_err_t ret = tas5805m_get_power_state(&state);
if (ret != ESP_OK) {
    ESP_LOGE("TAS5805M", "Failed to get power state");
} else {
    ESP_LOGI("TAS5805M", "Power state: %d", state);
}
```
DAC will auto-mute channels individually when no input signal is detected. This will give direct access to mute states per channel.

To get the automute state, use the `tas5805m_get_automute_state` function:

```cpp
bool is_r_muted, is_l_muted;
esp_err_t ret = tas5805m_get_automute_state(&is_r_muted, &is_l_muted);
if (ret != ESP_OK) {
    ESP_LOGE("TAS5805M", "Failed to get automute state");
} else {
    ESP_LOGI("TAS5805M", "Right automute: %s, Left automute: %s", is_r_muted ? "Muted" : "Unmuted", is_l_muted ? "Muted" : "Unmuted");
}
```

### Getting and Clearing Fault States

DAc has a powerful fault detection system hat allows it to self-diagnose issues with power, data signal, short circuits, overheating, etc.

The general pattern is, one should implement periodic checks of the fault registers, and in case there are any, act accordingly and reset the faults afterwards. Here is the possible pattern implementation:

```cpp
static void checkFaults()
{
    TAS5805M_FS_FREQ freq;
    uint8_t ratio;
    Tas5805m.getFsFreq(&freq);
    Tas5805m.getBckRatio(&ratio);
    
    TAS5805M_CTRL_STATE state;
    Tas5805m.getPowerState(&state);
    
    bool is_r_muted, is_l_muted;
    Tas5805m.getAutomuteState(&is_r_muted, &is_l_muted);
    
    ESP_LOGI(TAG, "FS Frequency: %s, BCK ratio: %d; Power state: %s; Automute: R: %d, L: %d", 
        tas5805m_map_fs_freq(freq), ratio, 
        tas5805m_map_amp_state(state), 
        is_r_muted, is_l_muted
    );
    
    TAS5805M_FAULT fault;
    Tas5805m.getFaultState(&fault);
    Tas5805m.decodeFaults(fault);

    if (fault.err0 || fault.err1 || fault.err2 || fault.ot_warn)
    {
        ESP_LOGI(TAG, "Clearing fault states");
        Tas5805m.clearFaultState();
    }
}
```

To get the fault states, use the `tas5805m_get_faults` function:

```cpp
TAS5805M_FAULT fault;
esp_err_t ret = tas5805m_get_faults(&fault);
if (ret != ESP_OK) {
    ESP_LOGE("TAS5805M", "Failed to get fault states");
} else {
    ESP_LOGI("TAS5805M", "Fault states: err0=%d, err1=%d, err2=%d, ot_warn=%d", fault.err0, fault.err1, fault.err2, fault.ot_warn);
}
```

To clear the fault states, use the `tas5805m_clear_faults` function:

```cpp
esp_err_t ret = tas5805m_clear_faults();
if (ret != ESP_OK) {
    ESP_LOGE("TAS5805M", "Failed to clear fault states");
}
```

### Decoding Fault Errors

To decode fault errors, use the `tas5805m_decode_faults` function:

```cpp
TAS5805M_FAULT fault = { .err0 = 1, .err1 = 0, .err2 = 0, .ot_warn = 1 };
tas5805m_decode_faults(fault);
```

## To Do

- [ ] - Implement EQ Bands as named enums to make things more clear
- [ ] - Implement soft clipping controls
- [ ] - Spread spectrum enable control
- [ ] - Test different modulation modes vs Switching frequency vs PVDD in terms of efficiency
- [ ] - Implement mixer granular controls
- [ ] - Digital audio level read control

## License

This library is licensed under the GPL v3 License. See the LICENSE file for more details.

## Contributing

Contributions are welcome! Please open an issue or submit a pull request on GitHub. Mind, this is a work in progress, and bugs are expected. Please don't hesitate to reach out at [Discord](https://discord.gg/PtnaAaQMpS), or directly at andriy@sonocotta.com

## Acknowledgements

This library is based on the ESP-IDF framework and the TAS5805M DAC documentation. I also used some part of the [Linux Kernel implementation](https://github.com/torvalds/linux/blob/master/sound/soc/codecs/tas5805m.c) of the driver, and also [my own implementation](https://github.com/sonocotta/tas5805m-driver-for-raspbian)

- [TAS5805M Datasheet](https://www.ti.com/lit/ds/symlink/tas5805m.pdf)
- [TAS5805M Process Flows](https://www.ti.com/lit/an/sloa263a/sloa263a.pdf)
- [TAS5805M Tuning Guide](https://www.ti.com/lit/an/slaa894/slaa894.pdf)
