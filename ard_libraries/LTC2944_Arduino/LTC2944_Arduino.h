//Custom library for interfacing with the LTC2944 chip using the Arduino


#ifndef LTC2944_Arduino_H
#define LTC2944_Arduino_H

#define LTC2944_I2C_ADDRESS 0x64 //Static address. Cannot be changed.
#define LTC2944_I2C_ALERT_RESPONSE  0x0C
//! @}


/*!
| Name                                              | Value |
| :------------------------------------------------ | :---: |
| LTC2944_STATUS_REG                                | 0x00  |
| LTC2944_CONTROL_REG                               | 0x01  |
| LTC2944_ACCUM_CHARGE_MSB_REG                      | 0x02  |
| LTC2944_ACCUM_CHARGE_LSB_REG                      | 0x03  |
| LTC2944_CHARGE_THRESH_HIGH_MSB_REG                | 0x04  |
| LTC2944_CHARGE_THRESH_HIGH_LSB_REG                | 0x05  |
| LTC2944_CHARGE_THRESH_LOW_MSB_REG                 | 0x06  |
| LTC2944_CHARGE_THRESH_LOW_LSB_REG                 | 0x07  |
| LTC2944_VOLTAGE_MSB_REG                           | 0x08  |
| LTC2944_VOLTAGE_LSB_REG                           | 0x09  |
| LTC2944_VOLTAGE_THRESH_HIGH_MSB_REG               | 0x0A  |
| LTC2944_VOLTAGE_THRESH_HIGH_LSB_REG               | 0x0B  |
| LTC2944_VOLTAGE_THRESH_LOW_MSB_REG                | 0x0C  |
| LTC2944_VOLTAGE_THRESH_LOW_LSB_REG                | 0x0D  |
| LTC2944_CURRENT_MSB_REG                           | 0x0E  |
| LTC2944_CURRENT_LSB_REG                           | 0x0F  |
| LTC2944_CURRENT_THRESH_HIGH_MSB_REG               | 0x10  |
| LTC2944_CURRENT_THRESH_HIGH_LSB_REG               | 0x11  |
| LTC2944_CURRENT_THRESH_LOW_MSB_REG                | 0x12  |
| LTC2944_CURRENT_THRESH_LOW_LSB_REG                | 0x13  |
| LTC2944_TEMPERATURE_MSB_REG                       | 0x14  |
| LTC2944_TEMPERATURE_LSB_REG                       | 0x15  |
| LTC2944_TEMPERATURE_THRESH_HIGH_REG               | 0x16  |
| LTC2944_TEMPERATURE_THRESH_LOW_REG                | 0x17  |
*/

/*! @name Registers
@{ */
// Registers
#define LTC2944_STATUS_REG                          0x00
#define LTC2944_CONTROL_REG                         0x01
#define LTC2944_ACCUM_CHARGE_MSB_REG                0x02
#define LTC2944_ACCUM_CHARGE_LSB_REG                0x03
#define LTC2944_CHARGE_THRESH_HIGH_MSB_REG          0x04
#define LTC2944_CHARGE_THRESH_HIGH_LSB_REG          0x05
#define LTC2944_CHARGE_THRESH_LOW_MSB_REG           0x06
#define LTC2944_CHARGE_THRESH_LOW_LSB_REG           0x07
#define LTC2944_VOLTAGE_MSB_REG                     0x08
#define LTC2944_VOLTAGE_LSB_REG                     0x09
#define LTC2944_VOLTAGE_THRESH_HIGH_MSB_REG         0x0A
#define LTC2944_VOLTAGE_THRESH_HIGH_LSB_REG         0x0B
#define LTC2944_VOLTAGE_THRESH_LOW_MSB_REG          0x0C
#define LTC2944_VOLTAGE_THRESH_LOW_LSB_REG          0x0D
#define LTC2944_CURRENT_MSB_REG                     0x0E
#define LTC2944_CURRENT_LSB_REG                     0x0F
#define LTC2944_CURRENT_THRESH_HIGH_MSB_REG         0x10
#define LTC2944_CURRENT_THRESH_HIGH_LSB_REG         0x11
#define LTC2944_CURRENT_THRESH_LOW_MSB_REG          0x12
#define LTC2944_CURRENT_THRESH_LOW_LSB_REG          0x13
#define LTC2944_TEMPERATURE_MSB_REG                 0x14
#define LTC2944_TEMPERATURE_LSB_REG                 0x15
#define LTC2944_TEMPERATURE_THRESH_HIGH_REG         0x16
#define LTC2944_TEMPERATURE_THRESH_LOW_REG          0x17
//! @}

/*!
| Command Codes                                 | Value     |
| :-------------------------------------------- | :-------: |
| LTC2944_AUTOMATIC_MODE                        | 0xC0      |
| LTC2944_SCAN_MODE                             | 0x80      |
| LTC2944_MANUAL_MODE                           | 0x40      |
| LTC2944_SLEEP_MODE                            | 0x00      |
| LTC2944_PRESCALAR_M_1                         | 0x00      |
| LTC2944_PRESCALAR_M_4                         | 0x08      |
| LTC2944_PRESCALAR_M_16                        | 0x10      |
| LTC2944_PRESCALAR_M_64                        | 0x18      |
| LTC2944_PRESCALAR_M_256                       | 0x20      |
| LTC2944_PRESCALAR_M_1024                      | 0x28      |
| LTC2944_PRESCALAR_M_4096                      | 0x30      |
| LTC2944_PRESCALAR_M_4096_2                    | 0x31      |
| LTC2944_ALERT_MODE                            | 0x04      |
| LTC2944_CHARGE_COMPLETE_MODE                  | 0x02      |
| LTC2944_DISABLE_ALCC_PIN                      | 0x00      |
| LTC2944_SHUTDOWN_MODE                         | 0x01      |
*/

/*! @name Command Codes
@{ */
// Command Codes
#define LTC2944_AUTOMATIC_MODE                  0xC0
#define LTC2944_SCAN_MODE                       0x80
#define LTC2944_MANUAL_MODE                     0x40
#define LTC2944_SLEEP_MODE                      0x00

#define LTC2944_PRESCALAR_M_1                   0x00
#define LTC2944_PRESCALAR_M_4                   0x08
#define LTC2944_PRESCALAR_M_16                  0x10
#define LTC2944_PRESCALAR_M_64                  0x18
#define LTC2944_PRESCALAR_M_256                 0x20
#define LTC2944_PRESCALAR_M_1024                0x28
#define LTC2944_PRESCALAR_M_4096                0x30
#define LTC2944_PRESCALAR_M_4096_2              0x31

#define LTC2944_ALERT_MODE                      0x04
#define LTC2944_CHARGE_COMPLETE_MODE            0x02

#define LTC2944_DISABLE_ALCC_PIN                0x00
#define LTC2944_SHUTDOWN_MODE                   0x01

//! @}

/*!
| Conversion Constants                              |  Value   |
| :------------------------------------------------ | :------: |
| LTC2944_CHARGE_lsb                                | 0.34  mAh|
| LTC2944_VOLTAGE_lsb                               | 1.068  mV|
| LTC2944_CURRENT_lsb                               |  29.3  uV|
| LTC2944_TEMPERATURE_lsb                           | 0.25    C|
| LTC2944_FULLSCALE_VOLTAGE                         |  23.6   V|
| LTC2944_FULLSCALE_CURRENT                         |  60    mV|
| LTC2944_FULLSCALE_TEMPERATURE                     | 510     K|

*/
/*! @name Conversion Constants
@{ */
const float LTC2944_CHARGE_lsb = 0.34E-3;
const float LTC2944_VOLTAGE_lsb = 1.068E-3;
const float LTC2944_CURRENT_lsb = 29.3E-6;
const float LTC2944_TEMPERATURE_lsb = 0.25;
const float LTC2944_FULLSCALE_VOLTAGE = 70;
const float LTC2944_FULLSCALE_CURRENT = 60E-3;
const float LTC2944_FULLSCALE_TEMPERATURE = 510;
//! @}


void LTC2944_initialize();

byte LTC2944_read_bit_8(uint8_t i2caddr, uint8_t regaddr);

word LTC2944_read_bit_16(uint8_t i2caddr, uint8_t regaddr);

void LTC2944_write_bit_8(uint8_t i2caddr, uint8_t regaddr, uint8_t datain);

void LTC2944_write_bit_16(uint8_t i2caddr, uint8_t regaddr, uint16_t datain);

void LTC2944_write_control_code(byte control_code);

double LTC2944_raw_to_voltage(word raw_voltage);

double LTC2944_raw_to_current(word raw_current, float resistor);

float LTC2944_raw_to_celcius(word raw_temperature);

double LTC2944_raw_to_mAh(word raw_charge, float resistor, uint16_t prescalar);

#endif