#pragma once
#include <Arduino.h>

// #include "Timer.h"

class SimpleLed
{
private:
    uint8_t _pin;
    bool _state;
    uint16_t _tmr_blink_time, _blink_i;

public:
    SimpleLed(uint8_t pin);
    ~SimpleLed();
    bool blink(uint16_t blink_time);
    bool blink(uint16_t blink_time, uint16_t k);
    void reset();

    /**
     * @brief Toggles the state of the LED
     *
     * Toggles the state of the LED between on and off.
     * This function does not return a value.
     */
    void toggle()
    {
        _state = !_state;
        digitalWrite(_pin, _state);
    }
    /**
     * @brief Sets the state of the LED
     *
     * Sets the state of the LED to the specified value.
     * This function does not return a value.
     *
     * @param state The state to be set (true for on, false for off)
     */
    void set(bool state)
    {
        _state = state;
        digitalWrite(_pin, _state);
    }
    
    /// @brief Returns the current state of the LED
    ///
    /// Returns the current state of the LED, true for on and false for off.
    bool getState()
    {
        return _state;
    }
};

SimpleLed::SimpleLed(uint8_t pin)
{
    _pin = pin;
    _state = 0;
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, _state);
}

SimpleLed::~SimpleLed()
{
}

/**
 * @brief Causes the LED to blink at a specified interval
 *
 * @param blink_time The time interval between blinks in milliseconds
 *
 * @return The current state of the LED (true if on, false if off)
 */

bool SimpleLed::blink(uint16_t blink_time)
{
    if ((uint16_t)millis() - _tmr_blink_time >= blink_time)
    {
        _tmr_blink_time = millis();
        toggle();
        _blink_i = 0;
    }
    return _state;
}

/**
 * @brief Causes the LED to blink at a specified interval, with a specified number of blinks
 *
 * @param blink_time The time interval between blinks in milliseconds
 * @param k The number of blinks
 *
 * @return true if the LED is still blinking, false if the number of blinks has been reached
 */
bool SimpleLed::blink(uint16_t blink_time, uint16_t k)
{
    if (_blink_i == k)
    {
        _blink_i = 0;
        return false;
    }
    if ((uint16_t)millis() - _tmr_blink_time >= blink_time)
    {
        _tmr_blink_time = millis();
        toggle();
        if (!_state)
        {
            _blink_i++;
        }
    }
    return true;
}

/**
 * @brief Resets the LED to its initial state
 *
 * Resets the LED to its initial state (off) and resets the blink timer and counter.
 */
void SimpleLed::reset()
{
    _tmr_blink_time = millis();
    _blink_i = 0;
    set(0);
}
