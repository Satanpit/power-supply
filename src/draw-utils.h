#pragma once

#include <Arduino.h>

#include <TFT_eSPI.h>

class DrawUtils {
public:
    DrawUtils(TFT_eSPI *display) {
        this->display = display;
    }

    void drawLabel(String str, int x, int y, uint16_t color, uint16_t bg, bool fill = true, int padding = 12) {
        int corr = 2;

        this->display->setFreeFont(&FreeSans12pt7b);
        this->display->setTextSize(1);
        this->display->setTextColor(color);
        this->display->setTextDatum(CC_DATUM);

        uint16_t w = this->display->textWidth(str);
        uint16_t h = 18;

        if (fill) {
            this->display->fillRoundRect(x, y, w + padding + corr, h + padding + corr, 0, bg);
        } else {
            this->display->drawRoundRect(x, y, w + padding + corr, h + padding + corr, 0, bg);
        }

        this->display->setCursor(x + (padding / 2), y + h + (padding / 2));
        this->display->print(str);
    }
private:
    TFT_eSPI *display;
};

