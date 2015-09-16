/*
    Copyright (C) <2015>  by Robin  lyp40293@gmail.com

    Arduino program
    
    Test the arm with a single servo. Try different speed and angle.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "Servo.h"

int led = 4;

int processSetupConnection(char *rec_buf)
{
    // digitalWrite(led, HIGH);
    char checksum = (char)0;
    int i = 0; 
    for (i = 0; i < 11; i++)
        checksum += rec_buf[i];

    char send_buf[8];
    send_buf[0] = (char)0xBA;    
    send_buf[1] = (char)0xDC;
    send_buf[2] = (char)0x0E;
    send_buf[3] = (char)0xFF;
    send_buf[4] = rec_buf[4];
    send_buf[5] = rec_buf[5];
    send_buf[6] = checksum;
    send_buf[7] = (char)0xAB;
    Serial.write(send_buf, 8);

    return 0;
}

int processSetAngle(char *rec_buf)
{
    digitalWrite(led, HIGH);   // can be put anywhere need a led to show signal
    char checksum = (char)0;
    int i = 0; 
    for (i = 0; i < 11; i++)
        checksum += rec_buf[i];

    char send_buf[8];
    send_buf[0] = (char)0xBA;    
    send_buf[1] = (char)0xDC;
    send_buf[2] = (char)0x0E;
    send_buf[3] = (char)0x0F;
    send_buf[4] = rec_buf[4];
    send_buf[5] = rec_buf[5];
    send_buf[6] = checksum;
    send_buf[7] = (char)0xAB;
    Serial.write(send_buf, 8);

    float angle = 0.0;
    char buf[4];
    for(i = 0; i < 4; i++)
        buf[i] = rec_buf[i+6];
    memcpy(&angle, buf, sizeof(float));
    angle = map(angle, -90,90,0,180);
    digitalWrite(led, HIGH);
    servo.write(angle);
    // delay(20);

    return 0;
}

int processSetSpeed(char *rec_buf)
{

}

void listenToHost()
{
    char rec_buf[11];
    if(Serial.available() > 0){
        Serial.readBytes(rec_buf, 11);
        if (rec_buf[0] == (char)0xAB && rec_buf[1] == (char)0xCD && rec_buf[2] == (char)0xE0){
            if(rec_buf[3] == (char)0xFF)
                processSetupConnection(rec_buf);
            else if(rec_buf[3] == (char)0x0F)
                processSetAngle(rec_buf);
            else if(rec_buf[3] == (char)0xF0)
                processSetSpeed(rec_buf);
            else
                ;
        }
        else
            ;

    }
}

void setup()
{
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  servo.attach(9);
}

void loop()
{
  
    listenToHost();
  
    delay(10);
    digitalWrite(led, LOW);
    delay(10);
}




