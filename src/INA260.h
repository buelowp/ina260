/*
 * Copyright 2018 Matthew Brush <mbrush@codebrainz.ca>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef INA260_H
#define INA260_H

#include <stdint.h>

#define INA260_DEFAULT_ADDRESS    0x40

class INA260 {
public:
    enum RegisterAddress {
        CONFIGURATION_REGISTER = 0x00,
        CURRENT_REGISTER = 0x01,
        BUS_VOLTAGE_REGISTER = 0x02,
        POWER_REGISTER = 0x03,
        MASK_ENABLE_REGISTER = 0x06,
        ALERT_LIMIT_REGISTER = 0x07,
        MANUFACTURER_ID_REGISTER = 0xFE,
        DIE_ID_REGISTER = 0xFF,
    };

    enum AveragingMode {
        AVG_1 = 0b000,
        AVG_4 = 0b001,
        AVG_16 = 0b010,
        AVG_64 = 0b011,
        AVG_128 = 0b100,
        AVG_256 = 0b101,
        AVG_512 = 0b110,
        AVG_1024 = 0b111,
    };

    enum VoltageConversionTime {
        VBUSCT_140US = 0b000,
        VBUSCT_204US = 0b001,
        VBUSCT_332US = 0b010,
        VBUSCT_588US = 0b011,
        VBUSCT_1_1MS = 0b100,
        VBUSCT_2_116MS = 0b101,
        VBUSCT_4_156MS = 0b110,
        VBUSCT_8_244MS = 0b0111,
    };

    enum CurrentConversionTime {
        ISHCT_140US = 0b000,
        ISHCT_204US = 0b001,
        ISHCT_332US = 0b010,
        ISHCT_588US = 0b011,
        ISHCT_1_1MS = 0b100,
        ISHCT_2_116MS = 0b101,
        ISHCT_4_156MS = 0b110,
        ISHCT_8_244MS = 0b111,
    };

    enum OperatingMode {
        MODE_POWER_DOWN1 = 0b000,
        MODE_ISH_TRIGGERED = 0b0001,
        MODE_VBUS_TRIGGERED = 0b010,
        MODE_ISH_VBUS_TRIGGERED = 0b011,
        MODE_POWER_DOWN2 = 0b0100,
        MODE_ISH_CONTINUOUS = 0b101,
        MODE_VBUS_CONTINUOUS = 0b110,
        MODE_ISH_VBUS_CONTINUOUS = 0b111,
    };

    union ConfigurationRegister {
        struct __attribute__((packed)) {
            uint16_t mode : 3;
            uint16_t ishct : 3;
            uint16_t vbusct : 3;
            uint16_t avg : 3;
            uint16_t : 3;
            uint16_t rst : 1;
        };
        uint16_t rawValue;
    };

    union MaskEnableRegister {
        struct __attribute__((packed)) {
            uint16_t len : 1;
            uint16_t apol : 1;
            uint16_t ovf : 1;
            uint16_t cvrf : 1;
            uint16_t aff : 1;
            uint16_t : 5;
            uint16_t cnvr : 1;
            uint16_t pol : 1;
            uint16_t bul : 1;
            uint16_t bol : 1;
            uint16_t ucl : 1;
            uint16_t ocl : 1;
        };
        uint16_t rawValue;
    };

    union DieIdRegister {
        struct __attribute__((packed)) {
            uint16_t rid : 4;
            uint16_t did : 12;
        };
        uint16_t rawValue;
    };

    INA260(uint8_t addr = INA260_DEFAULT_ADDRESS);
    ~INA260();

    double power();     /* In mW */
    double current();   /* In mA */
    double voltage();   /* in mV */
    bool reset();
    bool manufacturerId(char mfgStr[3]);

    bool writeConfigurationRegister(ConfigurationRegister &reg) {
        return writeRegister(CONFIGURATION_REGISTER, reg.rawValue);
    }

    bool readMaskEnableRegister(MaskEnableRegister &reg) {
        return readRegister(MASK_ENABLE_REGISTER, reg.rawValue);
    }

    bool writeMaskEnableRegister(MaskEnableRegister &reg) {
        return writeRegister(MASK_ENABLE_REGISTER, reg.rawValue);
    }

    bool writeAlertLimitRegister(uint16_t value) {
      return writeRegister(ALERT_LIMIT_REGISTER, value);
    }

    bool readDieIdRegister(DieIdRegister &reg) {
      return readRegister(DIE_ID_REGISTER, reg.rawValue);
    }

    bool readConfigurationRegister(ConfigurationRegister &reg) {
        return readRegister(CONFIGURATION_REGISTER, reg.rawValue);
    }

    bool readAlertLimitRegister(uint16_t &value) {
        return readRegister(ALERT_LIMIT_REGISTER, value);
    }

    bool clearAlert() {
      MaskEnableRegister reg{};
      return readMaskEnableRegister(reg);
    }

private:
    bool readRegister(uint8_t reg, uint16_t &value);
    bool writeRegister(uint8_t reg, uint16_t value);

    bool readCurrentRegister(uint16_t &value) {
        return readRegister(CURRENT_REGISTER, value);
    }

    bool readBusVoltageRegister(uint16_t &value) {
        return readRegister(BUS_VOLTAGE_REGISTER, value);
    }

    double readPowerRegister(uint16_t &value) {
        return readRegister(POWER_REGISTER, value);
    }

    uint8_t m_address;
};

#endif // INA260_H
