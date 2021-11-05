#pragma once

#include <Arduino.h>

#include <I2C_eeprom.h>

class Memory {
private:
    I2C_eeprom *eeprom;
public:
    Memory(const uint8_t deviceAddress) {
        this->eeprom = new I2C_eeprom(deviceAddress, I2C_DEVICESIZE_24LC64);
    }

    bool begin() {
        return this->eeprom->begin();
    }

    float getFloat(int address, float defaultValue = 0.0) {
        byte raw[4];

        for(byte i = 0; i < sizeof(raw); i++) raw[i] = this->eeprom->readByte(address + i);
        float &num = (float&)raw;

        return num > 0 ? num : defaultValue;
    }

    void setFloat(int address, float value) {
        byte raw[4];
        (float&)raw = value;
        for(byte i = 0; i < 4; i++) this->eeprom->writeByte(address + i, raw[i]);
    }

    void setBool(int address, bool value) {
        this->eeprom->updateByte(address, value);
    }

    bool getBool(int address) {
        return this->eeprom->readByte(address);
    }
};
