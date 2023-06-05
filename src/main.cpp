#include "config.h"

RH_RF69 rf69(RFM69_CS, RFM69_INT);

void setup() {

  Serial.begin(115200);

  pinMode(LED, OUTPUT);
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);

  while (!Serial)
  {
    delay(1);
  }

  if(Serial)
  {
    Serial.println("Serial Monitor Ready!");
  }
  else
  {
    Serial.println("Serial Monitor Not Ready!");
  }

  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);

  if (!rf69.init()) {
    Serial.println("RFM69 Radio init() Failed.");
    while (1);
  }
  else
  {
    Serial.println("RFM69 Radio init() OK!");
  }

  if (!rf69.setFrequency(RF69_FREQ)) 
  {
    Serial.println("setFrequency failed.");
  }
  else
  {
    Serial.println("setFrequency success.");
  }

  rf69.setTxPower(18, true);

  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};

  rf69.setEncryptionKey(key);

  Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");

}

void loop() {

    if (rf69.available())
    {
      uint8_t buf[120];
      uint8_t len = sizeof(buf);

      if (rf69.recv(buf, &len))
      {
          if (!len)
          {
            return;
          }
        Serial.println((char *)buf);
        buf[len] = 0;

        // Serial.print(":3\n");
        // Serial.print("10000000000");
        // Serial.print(",");
        // Serial.println("10000000000");

        // Serial.print("Signal Strength: ");
        // Serial.println(rf69.lastRssi(), DEC);
        // Serial.println("");
      }
    }

    char data[RH_RF69_MAX_MESSAGE_LEN];
    static char endMarker = '\n';
    char receivedChar;
    int ndx = 0;
    memset(data, 32, sizeof(data));

    bool dataReceived = false;

    while (Serial.available() > 0)
    {
      receivedChar = Serial.read();
      if (receivedChar == endMarker)
      {
        data[ndx] = '\0';
        continue;
      }

      data[ndx] = receivedChar;
      ndx++;
      if (ndx >= RH_RF69_MAX_MESSAGE_LEN)
      {
        break;
      }
      dataReceived = true;
    }

    if (dataReceived)
    {
        rf69.send((uint8_t *)data, sizeof(data));
        rf69.waitPacketSent();
    }

    memset(data, 32, sizeof(data));
}