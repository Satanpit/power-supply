#pragma once

#include <Arduino.h>

#include <TFT_eSPI.h>

#include <utils.h>

#define SIZE_SMALL 0
#define SIZE_MIDDLE 1

#define DEFAULT_AREA_WIDTH 110
#define DEFAULT_AREA_HEIGHT 28

#define GFXFF 1

class DrawArea {
public:
    DrawArea(TFT_eSPI *display) {
        this->display = display;
        this->sprite = new TFT_eSprite(this->display);

        this->sprite->setColorDepth(16);
    }

    void setSize(int width, int height, int size = SIZE_MIDDLE) {
        this->width = width;
        this->height = height;
        this->size = size;
    }

    void setPosition(int x, int y) {
        this->x = x;
        this->y = y;
    }

    void setColors(uint16_t valueColor, uint16_t labelColor) {
        this->valueColor = valueColor;
        this->labelColor = labelColor;
    }

    void setLabel(String label) {
        this->label = label;
    }

    void draw(float value, int decimal = 2) {
        this->sprite->createSprite(this->width, this->height);
        this->sprite->fillScreen(TFT_BLACK);

        this->sprite->setFreeFont(this->isSmall() ? &FreeSans12pt7b : &FreeSans18pt7b);
        this->sprite->setTextDatum(TL_DATUM);
        this->sprite->setTextColor(this->valueColor);
        
        if (value <= 9.999 && decimal != 0) {
            decimal = 3;
        }

        int fontW = this->sprite->textWidth(String(value, decimal));
        int fontH = this->sprite->fontHeight(GFXFF);

        this->sprite->setTextDatum(TR_DATUM);
        this->sprite->drawFloat(value, decimal, 85, 0, GFXFF);
        
        this->sprite->setFreeFont(this->isSmall() ? &FreeSans9pt7b : &FreeSans12pt7b);
        this->sprite->setTextDatum(TL_DATUM);
        this->sprite->setTextColor(this->labelColor);
        this->sprite->drawString(this->label, 90, this->isSmall() ? 4 : 8, GFXFF);

        this->sprite->pushSprite(this->x, this->y);
        this->sprite->deleteSprite();
    }
private:
    TFT_eSPI *display;
    TFT_eSprite *sprite;
    int width = DEFAULT_AREA_WIDTH;
    int height = DEFAULT_AREA_HEIGHT;
    int size = SIZE_MIDDLE;
    int x;
    int y;
    uint16_t valueColor;
    uint16_t labelColor;
    String label;

    bool isSmall() {
        return this->size == 0;
    }
};
