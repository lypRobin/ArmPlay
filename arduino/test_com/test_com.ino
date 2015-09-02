
int led = 4;

void setup()
{
  Serial.begin(9600);
  pinMode(led, OUTPUT);
}

void loop()
{
  char buf[5];

  if(Serial.available() > 0){
    Serial.readBytes(buf, 5);
    Serial.println(buf[0]);
    if (buf[0] == (char)0xAB && buf[1] == (char)0xCD && buf[2] == (char)0xE0 && buf[3] == (char)0x12 && buf[4] == (char)0xBA)
      digitalWrite(led, HIGH);
      char rbuf[5] = {0xBA, 0x12, 0xE0, 0xCD, 0xAB};
      Serial.write(rbuf, 5);
      // Serial.print(rbuf);
      // delay(100);
      // Serial.write(rbuf, 5);
      // delay(100);
      // Serial.write(rbuf, 5);

  }
  delay(500);
  digitalWrite(led, LOW);
  delay(500);
}

