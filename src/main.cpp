#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

#include <TFT_eSPI.h>
#include <GyverEncoder.h>
#include <I2C_eeprom.h>

#include <utils.h>
#include <memory.h>
#include <draw-area.h>
#include <channel.h>

#define INPUT_COLOR 0xFFFF
#define INPUT_COLOR_GREY 0x8410
#define OUT_COLOR 0xD700
#define OUT_COLOR_GREY 0x5B20
#define POWER_COLOR 0x5AE5
#define POWER_COLOR_GREY 0x2102

#define ENC_VOLTAGE_A_CLK PB1
#define ENC_VOLTAGE_A_DT PB0
#define ENC_VOLTAGE_A_SW PB2

#define ENC_CURRENT_A_CLK PB13
#define ENC_CURRENT_A_DT PB14
#define ENC_CURRENT_A_SW PB15

#define ENC_VOLTAGE_B_CLK PB8
#define ENC_VOLTAGE_B_DT PB7

#define ENC_CURRENT_B_CLK PC14
#define ENC_CURRENT_B_DT PC13

#define DAC_CHANNEL_A 0x60
#define DAC_CHANNEL_B 0x61

#define ADC_CANNEL_A 0x40
#define ADC_CANNEL_B 0x41

#define EEPROM_ADDRESS 0x50

#define ENCODERS_TIMER_INTERVAL 1000

Encoder encVoltageA(ENC_VOLTAGE_A_CLK, ENC_VOLTAGE_A_DT, ENC_VOLTAGE_A_SW);
Encoder encCurentA(ENC_CURRENT_A_CLK, ENC_CURRENT_A_DT, ENC_CURRENT_A_SW);

Encoder encVoltageB(ENC_VOLTAGE_B_CLK, ENC_VOLTAGE_B_DT);
Encoder encCurentB(ENC_CURRENT_B_CLK, ENC_CURRENT_B_DT);

TFT_eSPI display = TFT_eSPI();

Memory memory(EEPROM_ADDRESS);

Channel channelA(&display, &memory);
Channel channelB(&display, &memory);

void tickEncoders() {
  encVoltageA.tick();
  encCurentA.tick();
  
  encVoltageB.tick();
  encCurentB.tick();
};

void setup() {
  display.init();
  display.setRotation(3);
  display.fillScreen(TFT_BLACK);

  memory.begin();

  channelA.initEncoders(&encVoltageA, &encCurentA);
  channelA.setStartMemoryAddress(0);
  channelA.setLabel("A");
  channelA.begin(DAC_CHANNEL_A, ADC_CANNEL_A);
  channelA.calibrate(0.022);
  channelA.draw(0, 0);

  channelB.initEncoders(&encVoltageB, &encCurentB);
  channelB.setStartMemoryAddress(64);
  channelB.setLabel("B");
  channelB.begin(DAC_CHANNEL_B, ADC_CANNEL_B);
  channelB.calibrate(0.021);
  channelB.draw(165, 0);

  HardwareTimer *TickTimer = new HardwareTimer(TIM2);
  TickTimer->setOverflow(ENCODERS_TIMER_INTERVAL, MICROSEC_FORMAT);
  TickTimer->attachInterrupt(tickEncoders);
  TickTimer->resume();
}

void loop() {
  channelA.tick();
  channelB.tick();
}