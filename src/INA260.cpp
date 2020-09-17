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

#include <INA260.h>
#include <Arduino.h>
#include <Wire.h>

INA260::INA260(uint8_t addr) : m_address(addr)
{
    if (!Wire.isEnabled()) {
        Wire.begin();
    }
    reset();
}

INA260::~INA260()
{
}

bool INA260::readRegister(uint8_t reg, uint16_t &value) 
{
    Wire.beginTransmission(m_address);
    Wire.write(reg);
    int rval = Wire.endTransmission();
    Log.info("%s:%d: Wire tranmission to reg %d returned %d", __FUNCTION__, __LINE__, reg, rval);
    Wire.requestFrom(m_address, 2u);
    if (Wire.available() == 2) {
        const uint16_t msb = Wire.read();
        const uint16_t lsb = Wire.read();
        value = (msb << 8) | lsb;
        return true;
    }
    Log.info("%s:%d: Unable to read from register %d", __FUNCTION__, __LINE__, reg);
    return false;
}

bool INA260::writeRegister(uint8_t reg, uint16_t value) 
{
    Wire.beginTransmission(m_address);
    Wire.write(reg);
    Wire.write((value >> 8) & 0xFF);
    Wire.write(value & 0xFF);
    int rval = Wire.endTransmission();
    Log.info("%s:%d: Wire tranmission to reg %d returned %d", __FUNCTION__, __LINE__, reg, rval);
    return (rval == 0);
}

bool INA260::reset() 
{
    Log.info("%s:%d", __FUNCTION__, __LINE__);
    ConfigurationRegister reg{};
    reg.rst = 1;
    return writeConfigurationRegister(reg);
}

double INA260::power()
{
    uint16_t raw = 0;

    if (readPowerRegister(raw)) {
        return static_cast<double>(raw * 1.25);
    }

    return 0.0;
}

double INA260::current()
{
    uint16_t raw = 0;

    if (readCurrentRegister(raw)) {
        return static_cast<double>(raw * 1.25);
    }

    return 0.0;
}

double INA260::voltage()
{
    uint16_t raw = 0;

    if (readBusVoltageRegister(raw)) {
        return static_cast<double>(raw * 1.25);
    }

    return 0.0;
}

bool INA260::manufacturerId(char mfgStr[3])
{
    uint16_t value = 0;
    if (readRegister(MANUFACTURER_ID_REGISTER, value)) {
        mfgStr[0] = static_cast<char>((value >> 8) & 0xFF);
        mfgStr[1] = static_cast<char>(value & 0xFF);
        mfgStr[2] = '\0';
        return true;
    } else {
        return false;
    }
}
