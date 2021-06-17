#include <EEPROM.h>
#include <IRremote.h>

bool isOpen = false;

unsigned int currentStatus = 1;

#define R_LED_PIN 6
#define G_LED_PIN 10
#define B_LED_PIN 9

#define LOW_SPEED_PIN 5 // YELLOW CABLE
#define MEDIUM_SPEED_PIN 4 // ORANGE CABLE
#define HIGH_SPEED_PIN 3 // GREEN CABLE

#define RECEIVER_PIN 2

IRrecv receiver(RECEIVER_PIN);
decode_results decodedData;

void setup()
{
  receiver.enableIRIn();

  Serial.begin(9600);

  pinMode(R_LED_PIN, OUTPUT);
  pinMode(G_LED_PIN, OUTPUT);
  pinMode(B_LED_PIN, OUTPUT);

  pinMode(LOW_SPEED_PIN, OUTPUT);
  pinMode(MEDIUM_SPEED_PIN, OUTPUT);
  pinMode(HIGH_SPEED_PIN, OUTPUT);

  currentStatus = (unsigned int) EEPROM.read(0);
  isOpen = (boolean) EEPROM.read(1);

  if (isOpen)
    updateAllRelays();
  else
    closeAllRelays();
}

void closeAllRelays() {
  digitalWrite(LOW_SPEED_PIN, HIGH);
  digitalWrite(MEDIUM_SPEED_PIN, HIGH);
  digitalWrite(HIGH_SPEED_PIN, HIGH);

  changeLedColor(0, 0, 0);

  EEPROM.update(1, false);
}

void updateAllRelays() {
  closeAllRelays();

  if (!isOpen)
    return;

  if (currentStatus == 1) {
    changeLedColor(0, 150, 0);

    digitalWrite(LOW_SPEED_PIN, LOW);

  } else if (currentStatus == 2) {
    changeLedColor(0, 0, 150);

    digitalWrite(MEDIUM_SPEED_PIN, LOW);
  } else {
    changeLedColor(150, 0, 0);

    digitalWrite(HIGH_SPEED_PIN, LOW);
  }

  EEPROM.update(0, currentStatus);
  EEPROM.update(1, isOpen);
}

void changeLedColor(unsigned int r, unsigned int g, unsigned int b) {
  analogWrite(R_LED_PIN, r);
  analogWrite(G_LED_PIN, g);
  analogWrite(B_LED_PIN, b);
}

void loop()
{

  if (receiver.decode(&decodedData))
  {
    Serial.println(decodedData.value, HEX);


    if (decodedData.value == 0xF7C03F && !isOpen) // ON
    {
      isOpen = true;
      updateAllRelays();
    }
    else if (decodedData.value == 0xF740BF && isOpen) // OFF
    {
      isOpen = false;
      closeAllRelays();
    } else if (decodedData.value == 0xF700FF  && currentStatus != 3 && isOpen) // INCREASE SPEED
    {
      currentStatus++;
      updateAllRelays();
    }

    else if (decodedData.value == 0xF7807F && currentStatus != 1 && isOpen) // DECREASE SPEED
    {
      currentStatus--;
      updateAllRelays();

    } else if (decodedData.value == 0xF720DF && currentStatus != 1 && isOpen) // r
    {
      currentStatus = 1;
      updateAllRelays();

    } else if (decodedData.value == 0xF7A05F && currentStatus != 2 && isOpen) // g
    {
      currentStatus = 2;
      updateAllRelays();

    }
    else if (decodedData.value == 0xF7609F && currentStatus != 3 && isOpen) // b
    {
      currentStatus = 3;
      updateAllRelays();

    }

    receiver.resume();
  }
}
