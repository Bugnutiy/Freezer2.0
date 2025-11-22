#pragma once

/**
 * @brief Defines a macro for a timer that triggers after a specified time interval, with a specified code block to execute.
 * The timer uses the millis() function to measure the elapsed time.
 *
 * @param T The time interval in milliseconds.
 * @param CODE The code block to execute when the timer triggers.
 *
 * @return The macro does not return a value.
 */
#define TMR32(T, CODE)                      \
  {                                         \
    static uint32_t tmr##T = -T;            \
    if (((uint32_t)millis() - tmr##T) >= T) \
    {                                       \
      tmr##T = millis();                    \
      CODE                                  \
    }                                       \
  }

/**
 * @brief Defines a macro for a timer that triggers after a specified time interval, with a specified code block to execute.
 * The timer uses the millis() function to measure the elapsed time.
 *
 * @param T The time interval in milliseconds.
 * @param CODE The code block to execute when the timer triggers.
 *
 * @return The macro does not return a value.
 */
#define TMR16(T, CODE)                      \
  {                                         \
    static uint16_t tmr##T = -T;            \
    if (((uint16_t)millis() - tmr##T) >= T) \
    {                                       \
      tmr##T = millis();                    \
      CODE                                  \
    }                                       \
  }

/**
 * @brief Defines a macro for a timer that triggers after a specified time interval, with a specified code block to execute.
 * The timer uses the millis() function to measure the elapsed time.
 *
 * @param T The time interval in milliseconds.
 * @param CODE The code block to execute when the timer triggers.
 *
 * @return The macro does not return a value.
 */
#define TMR8(T, CODE)                      \
  {                                        \
    static uint8_t tmr##T = -T;            \
    if (((uint8_t)millis() - tmr##T) >= T) \
    {                                      \
      tmr##T = millis();                   \
      CODE                                 \
    }                                      \
  }

/**
 * @brief Defines a macro for a timer that triggers after a specified time interval, with a specified code block to execute.
 * The timer uses the millis() function to measure the elapsed time.
 *
 * @param T The time interval in milliseconds.
 * @param CODE The code block to execute when the timer triggers.
 *
 * @return The macro does not return a value.
 */
#define TMR8_S(T, CODE)            \
  {                                \
    static uint8_t tmr##T = -T;    \
    uint8_t time = millis();       \
    if ((time - tmr##T) >= T)      \
    {                              \
      while ((time - tmr##T) >= T) \
        tmr##T += T;               \
      CODE                         \
    }                              \
  }

/**
 * @brief Defines a macro for a timer that triggers after a specified time interval, with a specified code block to execute.
 * The timer uses the millis() function to measure the elapsed time.
 *
 * @param T The time interval in milliseconds.
 * @param CODE The code block to execute when the timer triggers.
 *
 * @return The macro does not return a value.
 */
#define TMR16_S(T, CODE)           \
  {                                \
    static uint16_t tmr##T = -T;   \
    uint16_t time = millis();      \
    if ((time - tmr##T) >= T)      \
    {                              \
      while ((time - tmr##T) >= T) \
        tmr##T += T;               \
      CODE                         \
    }                              \
  }

/**
 * @brief Defines a macro for a timer that triggers after a specified time interval, with a specified code block to execute.
 * The timer uses the millis() function to measure the elapsed time.
 *
 * @param T The time interval in milliseconds.
 * @param CODE The code block to execute when the timer triggers.
 *
 * @return The macro does not return a value.
 */
#define TMR32_S(T, CODE)           \
  {                                \
    static uint32_t tmr##T = -T;   \
    uint32_t time = millis();      \
    if ((time - tmr##T) >= T)      \
    {                              \
      while ((time - tmr##T) >= T) \
        tmr##T += T;               \
      CODE                         \
    }                              \
  }

//----------------------------------------------------------------
#define TMR8_NEXT(T, CODE)                 \
  {                                        \
    static uint8_t tmr##T = 0;             \
    if (((uint8_t)millis() - tmr##T) >= T) \
    {                                      \
      tmr##T = millis();                   \
      CODE                                 \
    }                                      \
  }

#define TMR16_NEXT(T, CODE)                 \
  {                                         \
    static uint16_t tmr##T = 0;             \
    if (((uint16_t)millis() - tmr##T) >= T) \
    {                                       \
      tmr##T = millis();                    \
      CODE                                  \
    }                                       \
  }

#define TMR32_NEXT(T, CODE)                 \
  {                                         \
    static uint32_t tmr##T = 0;             \
    if (((uint32_t)millis() - tmr##T) >= T) \
    {                                       \
      tmr##T = millis();                    \
      CODE                                  \
    }                                       \
  }

#define TMR8_NEXT_S(T, CODE)       \
  {                                \
    static uint8_t tmr##T = 0;     \
    uint8_t time = millis();       \
    if ((time - tmr##T) >= T)      \
    {                              \
      while ((time - tmr##T) >= T) \
        tmr##T += T;               \
      CODE                         \
    }                              \
  }

#define TMR16_NEXT_S(T, CODE)      \
  {                                \
    static uint16_t tmr##T = 0;    \
    uint16_t time = millis();      \
    if ((time - tmr##T) >= T)      \
    {                              \
      while ((time - tmr##T) >= T) \
        tmr##T += T;               \
      CODE                         \
    }                              \
  }

#define TMR32_NEXT_S(T, CODE)      \
  {                                \
    static uint32_t tmr##T = 0;    \
    uint32_t time = millis();      \
    if ((time - tmr##T) >= T)      \
    {                              \
      while ((time - tmr##T) >= T) \
        tmr##T += T;               \
      CODE                         \
    }                              \
  }
//----------------------------------------------------------------