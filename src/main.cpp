/*
Refrigerator with two chambers and one compressor. There is an anti-frostbite heater in the refrigerator compartment.
How the refrigerator works:
1) The compressor starts if the temperature in the refrigerator or freezer is higher than normal, while the Nofrost system is not working.
2) The Nofrost system asks to be started about once a day or when the temperature reaches too low.
3) The number of hours worked without Nofrost is stored in the EEPROM
4) When Nofrost is turned on, it must be turned off after a certain time or when the desired temperature is reached.
*/
#include <Arduino.h>
#define MY_DEBUG
#include "My_Debug.h"
#include "Timer.h"
#include <GyverDS18Single.h>
#include "Relay.h"
#include "SimpleLed.h"
#include <EEManager.h>
#include <GyverButton.h>
#include <GyverEncoder.h>

Encoder encoder(2, 3, 11); // clk, dt, sw
void isrCLK()
{
  encoder.tick(); // отработка в прерывании
}
void isrDT()
{
  encoder.tick(); // отработка в прерывании
}

#define PIN_SENS_FRIDGE 8  // было 2
#define PIN_SENS_FREEZER 9 // было 3

#define PIN_COMPRESSOR 4
#define PIN_NO_FROST 5

#define PIN_LED_FRIDGE 6
#define PIN_LED_FREEZER 7

#define PIN_BUTTON 10

#define NO_FROST_ON_TEMP_TRIGGER_DEFAULT -15
#define NO_FROST_ON_TEMP_TRIGGER_MAX 5
#define NO_FROST_ON_TEMP_TRIGGER_MIN -20

#define NO_FROST_OFF_TEMP_TRIGGER_DEFAULT 5
#define NO_FROST_OFF_TEMP_TRIGGER_MAX 20
#define NO_FROST_OFF_TEMP_TRIGGER_MIN -15

#define NO_FROST_OFF_TEMP_FREEZER -10

#define NO_FROST_ON_TIME 24 // часа
#define NO_FROST_OFF_TIME 2 // часа

const uint32_t MAX_WORK_TIME = ((uint32_t)1000 * 60 * 60 * 4); // 4 часа работает
const uint32_t BREAK_TIME = ((uint32_t)1000 * 60 * 20);        // Перерыв 20 минут
const uint32_t BREAK_RESET = ((uint32_t)1000 * 60 * 10);       // Естесственный перерыв

const uint32_t RELAY_CHANGE_TIME = ((uint32_t)1000 * 60 * 10); // Как часто можно переключать реле

const uint32_t hour = (uint32_t)1000 * 60 * 60;

#define TEMP_FREEZER_REQ_MIN -40
#define TEMP_FREEZER_REQ_MAX -10
#define TEMP_FREEZER_DEFAULT_RANGE 1
#define TEMP_FREEZER_REQ_MAX_RANGE 10
#define TEMP_FREEZER_REQ_MIN_RANGE 1
// #define TEMP_FREEZER_EXTRA -5

#define TEMP_FRIDGE_REQ_MIN -20
#define TEMP_FRIDGE_REQ_MAX 20
#define TEMP_FRIDGE_DEFAULT_RANGE 1
#define TEMP_FRIDGE_REQ_MAX_RANGE 10
#define TEMP_FRIDGE_REQ_MIN_RANGE 1

#define EEKEY 'a'

#pragma pack(push, 1)
struct Settings
{
  // Настройки холодильной камеры
  int8_t FridgeTempReq = 5,
         FridgeTempRange = TEMP_FRIDGE_DEFAULT_RANGE;

  // Настройки морозилки
  int8_t FeezerTempReq = -24,
         FreezerTempRange = TEMP_FRIDGE_DEFAULT_RANGE;

  // Настройки NoFrost
  int8_t NofrostTempOn = NO_FROST_ON_TEMP_TRIGGER_DEFAULT,
         NofrostTempOff = NO_FROST_OFF_TEMP_TRIGGER_DEFAULT;
};
uint8_t NoFrostHours = 24;
#pragma pack(pop)

Settings settings;
EEManager EE_Settings(settings);
EEManager EE_NoFrostHours(NoFrostHours);

GButton ExtraButton(PIN_BUTTON, HIGH_PULL, NORM_OPEN);
Relay compressor(PIN_COMPRESSOR, 0, RELAY_CHANGE_TIME);
Relay noFrost(PIN_NO_FROST, 0, 5000);

SimpleLed ledFridge(PIN_LED_FRIDGE);
SimpleLed ledFreezer(PIN_LED_FREEZER);
SimpleLed ledBuiltin(13);

GyverDS18Single sensorFridge(PIN_SENS_FRIDGE);
GyverDS18Single sensorFreezer(PIN_SENS_FREEZER);

int8_t tempFridge = 0, tempFreezer = 0;

uint8_t worker_mode = 0;
bool needFreezer = false, needFridge = false, needNofrost = false, needRest = false;

uint32_t timerNofrostOff = 0;
void setup()
{
  DD_INIT(115200);
  EE_Settings.begin(0, EEKEY);
  EE_NoFrostHours.begin(sizeof(settings) + 1, EEKEY);
  compressor.on();
  noFrost.off();
  compressor.resetTimer(1);
  noFrost.resetTimer(1);
  compressor.tick();
  noFrost.tick();
  attachInterrupt(0, isrCLK, CHANGE);
  attachInterrupt(1, isrDT, CHANGE);
  sensorFreezer.requestTemp();

  while (!sensorFreezer.ready())
  {
    ledFreezer.blink(200);
    ledBuiltin.blink(200);
    DD("sensorFreezer.requestTemp()", 1000);
    TMR16_NEXT(50000, break;)
    TMR16_NEXT(1000, sensorFreezer.requestTemp();)
  }
  if (sensorFreezer.readTemp())
    tempFreezer = sensorFreezer.getTempInt();
  else
    DD("Error sensorFreezer.readTemp()  Setup");
  ledFreezer.set(0);
  sensorFridge.requestTemp();
  while (!sensorFridge.ready())
  {
    ledFridge.blink(200);
    ledBuiltin.blink(200);
    DD("sensorFridge.requestTemp()", 1000);
    TMR16_NEXT(50000, break;)
    TMR16_NEXT(1000, sensorFridge.requestTemp();)
  }
  if (sensorFridge.readTemp())
    tempFridge = sensorFridge.getTempInt();
  else
    DD("Error sensorFridge.readTemp()  Setup");
  ledFridge.set(0);
  sensorFreezer.requestTemp();
}

void tempUpdate()
{
  if (sensorFreezer.ready())
  {
    if (sensorFreezer.readTemp())
      tempFreezer = sensorFreezer.getTempInt();
    else
      DD("Error sensorFreezer.readTemp()");
    sensorFreezer.requestTemp(); // запрос следующего измерения
  }
  if (sensorFridge.ready())
  {
    if (sensorFridge.readTemp())
      tempFridge = sensorFridge.getTempInt();
    else
      DD("Error sensorFridge.readTemp()");
    sensorFridge.requestTemp(); // запрос следующего измерения
  }
}
void NoFrostFunc()
{
  /*Должен ли работать NoFrost по температуре*/
  if (tempFridge < settings.NofrostTempOn)
  {
    needNofrost = true;
  }
  else if (tempFridge >= settings.NofrostTempOff)
  {
    needNofrost = false;
  }
  /*Должен ли работать NoFrost по таймеру*/
  if (!NoFrostHours)
  {
    needNofrost = true;
  }
  if ((millis() - timerNofrostOff > NO_FROST_OFF_TIME * hour) && noFrost.getState() )
  {
    needNofrost = false;
  }
  // Включаем реле Nofrost
  if (needNofrost && !compressor.getState() && !noFrost.getState())
  {
    noFrost.on();
    timerNofrostOff = millis();
  }
  // Выключаем реле Nofrost
  if (!needNofrost && noFrost.getState())
  {
    noFrost.off();
    NoFrostHours = 24;
  }
}
void loop()
{
  TMR16_NEXT(5000, tempUpdate(););
  TMR32_NEXT(hour, {
    if (NoFrostHours)
    {
      NoFrostHours--;
      EE_NoFrostHours.update();
    }
  });
  compressor.tick();
  noFrost.tick();
  EE_Settings.tick();
  EE_NoFrostHours.tick();
  /*Должен ли работать компрессор из-за морозилки*/
  if (tempFreezer > settings.FeezerTempReq + settings.FreezerTempRange)
  {
    needFreezer = true;
  }
  else if (tempFreezer <= settings.FeezerTempReq)
  {
    needFreezer = false;
  }
  /*Должен ли работать компрессор из-за холодильника*/
  if (tempFridge > settings.FridgeTempReq + settings.FridgeTempRange)
  {
    needFridge = true;
  }
  else if (tempFridge <= settings.FridgeTempReq)
  {
    needFridge = false;
  }
}
