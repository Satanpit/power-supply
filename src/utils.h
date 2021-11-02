#pragma once

#include <Arduino.h>

namespace Utils {
    float round(float value) {
        return round(value * 100) / 100;
    }

    float minMax(float value, float min = 0.0, float max = 1000.0) {
        if (value >= max) {
            return max;
        }

        if (value <= min) {
            return min;
        }

        return value;
    }

    String normalizeValue(float value, int decimal = 2) {
        if (value <= 9.999) {
            decimal = 3;
        }

        return String(value, decimal);
    }
}
