#include <SoftwareSerial.h>

// Arduino RX, Arduino TX
SoftwareSerial picSerial(8, 7);

void setup() {
  Serial.begin(9600);
  picSerial.begin(9600);
}

byte value = 0;

void loop() {
  picSerial.write(value);
  if (picSerial.available()) {
    char ackValue = picSerial.read();
    Serial.println(ackValue, DEC);
  } else {
    Serial.println("no data");
  }
  delay(200);
  value++;
}
