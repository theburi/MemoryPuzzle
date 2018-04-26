
#include <FastLED.h>
#include <CapacitiveSensor.h>

#define SEND_PIN 12
#define LED_DATA_PIN 13
#define ACTIVATE_SIGNAL_PIN 14
#define SOLVED_SIGNAL_PIN 15

#define READ_TIME 30
#define NUM_LEDS 10
#define SOUND_FREQ 1014
#define TOUCH_THRESHOLD 100
#define TOUCH_SENSORS 10

bool PUZZLE_STATE = 0;
int _recievePins[TOUCH_SENSORS] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
int _ledOrder[TOUCH_SENSORS] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
int _complexity = 10;

CapacitiveSensor _sensors[TOUCH_SENSORS] = {
    CapacitiveSensor(SEND_PIN, 2),
    CapacitiveSensor(SEND_PIN, 3),
    CapacitiveSensor(SEND_PIN, 4),
    CapacitiveSensor(SEND_PIN, 5),
    CapacitiveSensor(SEND_PIN, 6),
    CapacitiveSensor(SEND_PIN, 7),
    CapacitiveSensor(SEND_PIN, 8),
    CapacitiveSensor(SEND_PIN, 9),
    CapacitiveSensor(SEND_PIN, 10),
    CapacitiveSensor(SEND_PIN, 11)};

int _progressMarker = 0;
long start = millis();
bool firstPress = false;

CRGB leds[NUM_LEDS];
int touchSequence[TOUCH_SENSORS] = {2, 5, 1, 3, 8, 7, 4, 9, 6, 10};
bool touchResults[TOUCH_SENSORS];

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);

  FastLED.addLeds<NEOPIXEL, LED_DATA_PIN>(leds, NUM_LEDS);
  //_sensors[p].et_CS_Timeout_Millis(100);

  //SetUP Sequince  #
  pinMode(SOLVED_SIGNAL_PIN, OUTPUT);
  pinMode(ACTIVATE_SIGNAL_PIN, INPUT);

  digitalWrite(SOLVED_SIGNAL_PIN, LOW);
}

void loop()
{
  if (PUZZLE_STATE == 2)
  {
    delay(1000);
  }
  else if (PUZZLE_STATE == 0)
  {
    if (digitalRead(ACTIVATE_SIGNAL_PIN) == HIGH)
      PUZZLE_STATE = 1;
    else
      delay(100);
  }
  else
  {
    for (int p = _progressMarker; p < TOUCH_SENSORS; p++)
    {
      int _result = _sensors[touchSequence[p]].capacitiveSensor(READ_TIME);
      if (_result > TOUCH_THRESHOLD)
      {
        checkOrder(p);
        updateLEDs();
      }

      Serial.print(millis() - start); // check on performance in milliseconds
      Serial.print(" - Value ");      // tab character for debug window spacing
      Serial.print(_result);          // print sensor output 1
      Serial.print("\t");
    }
  }
}

void playMusic()
{
  for (int i = 0; i <= 80; i++)
  {
    analogWrite(A0, 200);
    delayMicroseconds(SOUND_FREQ);
    analogWrite(A0, 0);
    delayMicroseconds(SOUND_FREQ);
  }
}

void checkOrder(int pin)
{
  if (!firstPress && touchSequence[0] == pin)
  {
    firstPress = true;
    start = millis();
  }
  if (touchSequence[_progressMarker] == pin)
  {
    _progressMarker++;
    playMusic();
  }
  else
  {
    _progressMarker = 0;
  }

  if (_progressMarker >= _complexity)
  {
    PUZZLE_STATE = 2;
  }

  _complexity = (int)10 - (millis() - start) / 1000 / 300;
}

void updateLEDs()
{

  for (int p = 1; p < NUM_LEDS; p++)
  {
    if (p <= _progressMarker)
      leds[_ledOrder[p]] = CRGB(255, 100, 100);
    else
      leds[_ledOrder[p]] = CRGB(0, 0, 0);
  }

  if (_progressMarker == 0)
  {
    if (((start - millis()) / 1000 % 2) == 0)
      leds[0] = CRGB(255, 100, 100);
    else
      leds[0] = CRGB(25, 10, 10);
  }

  if (PUZZLE_STATE == 2)
    digitalWrite(SOLVED_SIGNAL_PIN, HIGH);

  FastLED.show();
}
