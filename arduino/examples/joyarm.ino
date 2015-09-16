/*
    Copyright (C) <2015>  by Robin  lyp40293@gmail.com

    Arduino program
    The arm control in arduino.

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
#define TOTAL_SERVO_NUM_MAX  20
#define ABS(x,y) (((x) >= (y)) ? ((x) - (y)) : (y) - (x))

#ifdef SINGLE_ARM
    #define PWM_ARM_0_SERVO_0    2   
    #define PWM_ARM_0_SERVO_1    3   
    #define PWM_ARM_0_SERVO_2    4   
    #define PWM_ARM_0_SERVO_3    5  
    #define PWM_ARM_0_SERVO_4    6   
    #define PWM_ARM_0_SERVO_5    7   
#endif

#ifdef DOUBLE_ARM
    #define PWM_ARM_0_SERVO_0    2   
    #define PWM_ARM_0_SERVO_1    3   
    #define PWM_ARM_0_SERVO_2    4   
    #define PWM_ARM_0_SERVO_3    5  
    #define PWM_ARM_0_SERVO_4    6   
    #define PWM_ARM_0_SERVO_5    7   
    #define PWM_ARM_1_SERVO_0    8   
    #define PWM_ARM_1_SERVO_1    9   
    #define PWM_ARM_1_SERVO_2    10   
    #define PWM_ARM_1_SERVO_3    11  
    #define PWM_ARM_1_SERVO_4    12   
    #define PWM_ARM_1_SERVO_5    13 
#endif

// class ArmServo
class ArmServo:public Servo{
private:
    int _angle;
public:
    ArmServo();
    int writeAngle(int angle);
    int getOldAngle();
};

ArmServo::ArmServo()
{
    _angle = 0;
}

int writeAngle(int angle)
{
    _angle = angle;
    write(angle);

    return 0;
}

int getOldAngle()
{
    return _angle;
}

// JoyArm
class JoyArm{
public:
    JoyArm();
    ~JoyArm();
private:
    int _totalServoNum;
public:
    ArmServo *_servo;

public:
    int setServoNum(int num);
    int getServoNum();
    int initArmServo();

};

JoyArm::JoyArm()
{
    _servo = null;
    _totalServoNum = 0;
}

JoyArm::~JoyArm()
{
    if(_servo != null)
        free(servo);
}

int setServoNum(int num)
{
    if(num <= TOTAL_SERVO_NUM_MAX)
        _totalServoNum = num;
    else
        return -1;

    return 0;

}

int getServoNum()
{
    return _totalServoNum;
}

int initArmServo()
{
    if(_totalServoNum == 0)
        return -1;

    _servo = (Servo*)malloc(sizeof(Servo) * _totalServoNum);
    if(_servo == null)
        return -1;

    return 0;
}

//================================================================
int led = 4;
int ledErr = 5;
const int defSpeed = 20;  // default delay time: 20ms
const int defDegreeStep = 3;  // default motion degree step: 3 deg
const int *initServoValue = {90, 90, 90, 90, 90, 90};

JoyArm *arm = null;
int totalArmNum = 0;

void showError()
{
    digitalWrite(ledErr, HIGH);  // ledErr light up.
}

int initArms(JoyArm *arm, char *rec_buf)
{
    totalArmNum = (int)((rec_buf[4] & 0xF0) >> 4);
    int servoNum = (int)rec_buf[5];
    if(arm == null)
        arm = (JoyArm*)malloc(sizeof(JoyArm) * armNum);

    if(armNum == DOUBLE_ARM){
        arm[0]._servo[0].attach(PWM_ARM_0_SERVO_0);
        arm[0]._servo[1].attach(PWM_ARM_0_SERVO_1);
        arm[0]._servo[2].attach(PWM_ARM_0_SERVO_2);
        arm[0]._servo[3].attach(PWM_ARM_0_SERVO_3);
        arm[0]._servo[4].attach(PWM_ARM_0_SERVO_4);
        arm[0]._servo[5].attach(PWM_ARM_0_SERVO_5);
        arm[1]._servo[0].attach(PWM_ARM_1_SERVO_0);
        arm[1]._servo[1].attach(PWM_ARM_1_SERVO_1);
        arm[1]._servo[2].attach(PWM_ARM_1_SERVO_2);
        arm[1]._servo[3].attach(PWM_ARM_1_SERVO_3);
        arm[1]._servo[4].attach(PWM_ARM_1_SERVO_4);
        arm[1]._servo[5].attach(PWM_ARM_1_SERVO_5);
    }

    int i = 0;
    for(i = 0; i < armNum; i++){
        if(arm[i].setServoNum(servoNum) < 0)
            showError();
        
        if(arm[i].initArmServo() < 0)
            showError();

        int j = 0;
        for(j = 0; j < arm[i].getServoNum(); j++)
            arm[i]._servo[j].writeAngle(initServoValue[j]);
    }

}

int processSetupConnection(char *rec_buf)
{
    // digitalWrite(led, HIGH);
    initArms(arm, rec_buf);
    
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
    
    int armIdx = (int)(rec_buf[4] & 0x0F);
    if(armIdx >= totalArmNum)
        return -1;

    int servoIdx = (int)rec_buf[5];
    if(servoIdx >= arm[armIdx]._totalServoNum)
        return -1;

    float angle = 0.0;
    char buf[4];
    for(i = 0; i < 4; i++)
        buf[i] = rec_buf[i+6];
    memcpy(&angle, buf, sizeof(float));
    angle = map(angle, -90,90,0,180);
    int oldAngle = arm[armIdx]._servo[servoIdx].getOldAngle();
    if(ABS(angle, oldAngle) >= defDegreeStep){
        
    }

    arm[armIdx]._servo[servoIdx].writeAngle(angle);


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
  pinMode(ledErr, OUTPUT);
  // servo.attach(9);
}

void loop()
{
  
    listenToHost();
  
    delay(10);
    digitalWrite(led, LOW);
    delay(10);
}