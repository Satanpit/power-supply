#pragma once

#include <Arduino.h>
#include <Wire.h>

#include <TFT_eSPI.h>

#include <GyverEncoder.h>
#include <Adafruit_MCP4728.h>
#include <INA226.h>

#include <utils.h>
#include <draw-utils.h>
#include <draw-area.h>
#include <memory.h>

#define GFXFF 1

#define COLOR_BASE_GREY 0x8410
#define COLOR_INPUT 0xFFFF
#define COLOR_INPUT_GREY 0x8410
#define COLOR_OUT 0xD700
#define COLOR_OUT_GREY 0x5B20
#define COLOR_POWER 0x5AE5
#define COLOR_POWER_GREY 0x2102

#define DAC_RESOLUTION 4095.0
#define DAC_VOLTAGE_REF 4.096

#define EEPROM_VOLTAGE_ADDR 0
#define EEPROM_CURRENT_ADDR 4
#define EEPROM_ENABLE_ADDR 8
#define EEPOROM_ENABLE_TRIGGER 9

class Channel {
public:
    Channel(TFT_eSPI *display, Memory *memory) {
        this->display = display;
        this->memory = memory;

        this->drawer = new DrawUtils(display);
        
        this->digital = new Adafruit_MCP4728();
        this->analog = new INA226(Wire);

        this->inputVoltageArea = new DrawArea(display);
        this->inputCurrentArea = new DrawArea(display);
        this->outputVoltageArea = new DrawArea(display);
        this->outputCurrentArea = new DrawArea(display);
        this->outputPowerArea = new DrawArea(display);

        this->inputVoltageArea->setLabel("V");
        this->inputVoltageArea->setColors(COLOR_INPUT, COLOR_INPUT_GREY);

        this->inputCurrentArea->setLabel("A");
        this->inputCurrentArea->setColors(COLOR_INPUT, COLOR_INPUT_GREY);

        this->outputVoltageArea->setLabel("V");
        this->outputVoltageArea->setColors(COLOR_OUT, COLOR_OUT_GREY);

        this->outputCurrentArea->setLabel("A");
        this->outputCurrentArea->setColors(COLOR_OUT, COLOR_OUT_GREY);

        this->outputPowerArea->setLabel("W");
        this->outputPowerArea->setColors(COLOR_POWER, COLOR_POWER_GREY);
        this->outputPowerArea->setSize(DEFAULT_AREA_WIDTH, DEFAULT_AREA_HEIGHT, SIZE_SMALL);
    }

    void setPosition(int x, int y) {
        this->x = x;
        this->y = y;
    }

    void begin(uint8_t dacAddress, uint8_t adcAddress) {
        this->digital->begin(dacAddress);
        this->analog->begin(adcAddress);
        this->analog->configure(INA226_AVERAGES_64);

        float voltage = this->memory->getFloat(this->startMemoryAddress + EEPROM_VOLTAGE_ADDR, this->inputVoltage);
        float current = this->memory->getFloat(this->startMemoryAddress + EEPROM_CURRENT_ADDR, this->inputCurrent);
        
        this->writeVoltage(voltage);
        this->writeCurrent(current);
    }

    void setStartMemoryAddress(int startMemoryAddress) {
        this->startMemoryAddress = startMemoryAddress;
    }

    void calibrate(float resistance) {
        this->resistance = resistance;
        this->analog->calibrate(resistance);
    }

    void setLabel(String label) {
        this->label = label;
    }

    void setMaxVoltage(float voltage) {
        this->maxVoltage = voltage;
    }

    void setMaxCurrent(float current) {
        this->maxCurrent = current;
    }

    void setVoltageSteps(float shortStep, float longStep) {
        this->voltageStepShort = shortStep;
        this->voltageStepLong = longStep;
    }

    void setCurrentSteps(float shortStep, float longStep) {
        this->currentStepShort = shortStep;
        this->currentStepLong = longStep;
    }

    void initEncoders(Encoder *voltage, Encoder *current) {
        this->encVoltage = voltage;
        this->encCurrent = current;

        this->encVoltage->setType(TYPE2);
        this->encCurrent->setType(TYPE2);
    }

    void readAnalogData() {
        float voltage = this->analog->readBusVoltage();
        float current = this->analog->readShuntCurrent();
        float power = this->analog->readBusPower();

        this->voltage = Utils::minMax(voltage, 0.0);
        this->current = Utils::minMax(current, 0.0);
        this->power = Utils::minMax(power, 0.0);
    }

    void writeVoltage(float voltage) {
        this->inputVoltage = Utils::minMax(voltage, 0.0, this->maxVoltage);
        this->digital->setChannelValue(MCP4728_CHANNEL_A, (DAC_RESOLUTION / this->maxVoltage) * this->inputVoltage);
    }

    void writeCurrent(float current) {
        this->inputCurrent = Utils::minMax(current, 0.0, this->maxCurrent);
        this->digital->setChannelValue(MCP4728_CHANNEL_B, (DAC_RESOLUTION / this->maxCurrent) * this->inputCurrent);
    }

    void drawOutputData() {
        this->outputVoltageArea->draw(this->voltage);
        this->outputCurrentArea->draw(this->current, 3);
        this->outputPowerArea->draw(this->power);
    }

    void drawInputData() {
        this->inputVoltageArea->draw(this->inputVoltage);
        this->inputCurrentArea->draw(this->inputCurrent, 3);
    }

    void drawLayout() {
        this->display->drawRect(this->x - 1, this->y - 1, 30, 30, COLOR_BASE_GREY);
        this->display->setFreeFont(&FreeSans9pt7b);
        this->display->setTextColor(COLOR_BASE_GREY);
        this->display->setTextDatum(CC_DATUM);
        this->display->drawString(this->label, this->x + 14, this->y + 11, GFXFF);
    
        this->display->setFreeFont(&FreeSans9pt7b);
        this->display->setTextDatum(TL_DATUM);
        this->display->setTextColor(COLOR_BASE_GREY);
        this->display->drawString("SET:", this->x, 52, GFXFF);
        this->display->setTextColor(COLOR_OUT_GREY);
        this->display->drawString("OUT:", this->x, 92, GFXFF);
    }

    void draw(int x, int y) {
        int paddingLeft = 45;

        this->x = x;
        this->y = y;

        this->inputVoltageArea->setPosition(this->x + paddingLeft, this->y + 40);
        this->inputCurrentArea->setPosition(this->x + paddingLeft, this->y);

        this->outputVoltageArea->setPosition(this->x + paddingLeft, this->y + 90);
        this->outputCurrentArea->setPosition(this->x + paddingLeft, this->y + 130);
        this->outputPowerArea->setPosition(this->x + paddingLeft, this->y + 167);

        this->drawLayout();
        
        this->inputVoltageArea->draw(this->inputVoltage);
        this->inputCurrentArea->draw(this->inputCurrent);

        this->drawOutputData();
    }

    void tick() {
        float voltage = this->inputVoltage;
        float current = this->inputCurrent;

        if (this->encVoltage->isRight()) {
            voltage += this->voltageStepShort;
        }

        if (this->encVoltage->isLeft()) {
            voltage -= this->voltageStepShort;
        }

        if (this->encVoltage->isFastR()) {
            voltage += this->voltageStepLong;
        }

        if (this->encVoltage->isFastL()) {
            voltage -= this->voltageStepLong;
        }

        if (this->encCurrent->isRight()) {
            current += this->currentStepShort;
        }

        if (this->encCurrent->isLeft()) {
            current -= this->currentStepShort;
        }

        if (this->encCurrent->isFastR()) {
            current += this->currentStepLong;
        }

        if (this->encCurrent->isFastL()) {
            current -= this->currentStepLong;
        }

        if (this->encVoltage->isTurn()) {
            this->writeVoltage(voltage);
            this->inputVoltageArea->draw(this->inputVoltage);
        }

        if (this->encCurrent->isTurn()) {
            this->writeCurrent(current);
            this->inputCurrentArea->draw(this->inputCurrent);
        }

        if (this->encVoltage->isClick()) {
            this->memory->setFloat(this->startMemoryAddress + EEPROM_VOLTAGE_ADDR, this->inputVoltage);
        }

        if (this->encCurrent->isClick()) {
            this->memory->setFloat(this->startMemoryAddress + EEPROM_CURRENT_ADDR, this->inputCurrent);
        }
        
        if (millis() - this->previousReadMillis) {
            this->previousReadMillis = millis();
            this->readAnalogData();
            this->drawOutputData();
        }
    }
private:
    TFT_eSPI *display;
    DrawUtils *drawer;
    Encoder *encVoltage;
    Encoder *encCurrent;
    INA226 *analog;
    Adafruit_MCP4728 *digital;

    DrawArea *inputVoltageArea;
    DrawArea *inputCurrentArea;
    DrawArea *outputVoltageArea;
    DrawArea *outputCurrentArea;
    DrawArea *outputPowerArea;

    Memory *memory;

    String label = "1";

    int x;
    int y;

    int analogReadInterval = 100;
    int startMemoryAddress = 0;

    float maxVoltage = 30.00;
    float maxCurrent = 3.00;
    float resistance = 0.01;

    float voltage = 0.00;
    float current = 0.000;
    float power = 0.00;

    float inputVoltage = 0.00;
    float inputCurrent = 0.000;

    float currentStepShort = 0.01;
    float currentStepLong = 0.05;
    float voltageStepShort = 0.1;
    float voltageStepLong = 0.5;

    unsigned long previousReadMillis = 0;
};
