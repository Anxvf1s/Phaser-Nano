/*
Press pushButton powerUP whilst in PHASER mode
Press pushButton2 powerDown whilst in PHASER mode
Long Press pushButton Toggle DIAGNOSTIC/PHASER mode
Long Press pushButton2 whilst DIAGNOSTIC switches to OVERLOAD mode
OVERLOAD mode you will see a short stall of the LEDs and it will begin a 10s self destruct sequence.
*/
#define PHASER 0
#define DIAGNOSTIC 1
#define OVERLOAD 2
#define DEAD_STATE 3

const int ledPin[10] = {
  13,11,10,9,8,6,5,4,3,2};
const int scanArray[5] = {
  0b1000000001, 0b0100000010, 0b0010000100, 0b0001001000, 0b0000110000};
const byte pushButton  = 7;
const byte pushButton2 = 12;
int spkr = A0;
byte lastVal;
byte lastVal2;
int index = 0;
byte state = PHASER;// initialises in phaser state
unsigned long timeStamp;
unsigned long timeStamp2;
unsigned long timeStamp3;

void setup()
{
  Serial.begin(115200);
  for (int i = 0; i < 11; i++)
  {
    pinMode(ledPin[i],OUTPUT);
  }
  pinMode(pushButton,INPUT_PULLUP);
  pinMode(pushButton2,INPUT_PULLUP);
}
void loop()
{
  byte val=digitalRead(pushButton);
  byte val2 = digitalRead(pushButton2);
  if (val != lastVal || val2 != lastVal2) delay(50);
  if(val==LOW && lastVal == HIGH && state == PHASER)
  {
    timeStamp = millis();
    index <<= 1;
    index |= 1;
    if (index > 0b1111111111) index = 0b1111111111;
    for (int i = 0; i < 10; i++)
    {
      digitalWrite(ledPin[i], bitRead(index, i));
      tone(spkr, 4000, 50);
    }
  }
  else if (val==HIGH && lastVal == LOW && state == PHASER)
  {
    if (millis() - timeStamp >= 2000UL)
    {
      state = DIAGNOSTIC;
      Serial.println(state);
    }
  }
  else if (val==HIGH && lastVal == LOW && state == DIAGNOSTIC)
  {
    if (millis() - timeStamp >= 2000UL)
    {
      state = PHASER;
      Serial.println(state);
    }
  }
  lastVal = val;
  if (val2 == LOW && lastVal2 == HIGH && state == PHASER)
  {
    timeStamp2 = millis();
    index >>= 1;
    tone(spkr, 4000, 50);//<<<<<<<<<<<<<<<<<<<<<<<<<< TONE ADDED
    Serial.print("index = ");
    Serial.println(index, BIN);
  }
  else if (val2 == LOW && lastVal2 == HIGH && state == DIAGNOSTIC)
  {
    timeStamp2 = millis();
  }
  else if (val2 == LOW && lastVal2 == LOW && state == DIAGNOSTIC)
  {
    if (millis() - timeStamp2 >= 2000UL)
    {
      delay(250);
      state = OVERLOAD;
      timeStamp3 = millis();
      Serial.println("overload");
    }
  }
  lastVal2 = val2;
  //
  if (state == PHASER)
  {
    for (int i = 0; i < 10; i++)
    {
      digitalWrite(ledPin[i], bitRead(index, i));
    }
  }
  else if (state == DIAGNOSTIC)
  {
    diagnogsticScan(false);
  }
  else if (state == OVERLOAD)
  {
    if (millis() - timeStamp3 > 10000UL)
    {
      diagnogsticScan(true);
    }
    else
    {
      diagnogsticScan(false);
    }
  }
  else if (state == DEAD_STATE)
  {
    // nothing to do here... I'm dead so hit reset to start over
  }
}
//
void diagnogsticScan(boolean faster)
{
  static unsigned long scanInterval = 80UL;
  static unsigned long scanStart;
  static byte scanIndex = 0;
  static byte increment = 1;
  if (millis() - scanStart >= scanInterval)
  {
    scanStart += scanInterval;
    for (int i = 0; i < 10; i++)
    {
      digitalWrite(ledPin[i], bitRead(scanArray[scanIndex], i));
    }
    scanIndex += increment;
    if (scanIndex == 0 || scanIndex == 4)
    {
      increment = -increment;
    }
    if (faster)
    {
      scanInterval--;
      if (scanInterval <= 0)
      {
        scanInterval = 80UL;
        state = DEAD_STATE;
        for (int i = 0; i < 10; i++)
        {
          digitalWrite(ledPin[i], HIGH);
        }
        delay(250);
        for (int i = 0; i < 10; i++)
        {
          digitalWrite(ledPin[i], LOW);
        }
      }
    }
    Serial.println(scanInterval);
  }
}
