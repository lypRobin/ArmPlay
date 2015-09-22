/*
    Copyright (C) <2015>  by Robin  lyp40293@gmail.com

    Arduino program
    The pArm control in arduino.

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
#define SPEED_LEVEL_MAX  3
#define SPEED_DELAY_TIME_MAX 30
#define SPEED_STEP_MAX 5

#define SPEED_LEVEL_0_DELAY_TIME    30
#define SPEED_LEVEL_0_STEP          1
#define SPEED_LEVEL_1_DELAY_TIME    20
#define SPEED_LEVEL_1_STEP          2
#define SPEED_LEVEL_2_DELAY_TIME    10
#define SPEED_LEVEL_2_STEP          3

#define ABS(x,y) (((x) >= (y)) ? ((x) - (y)) : ((y) - (x)))

#define DOUBLE_ARM 2

// #ifdef SINGLE_ARM
//     #define PWM_ARM_0_SERVO_0    2   
//     #define PWM_ARM_0_SERVO_1    3   
//     #define PWM_ARM_0_SERVO_2    4   
//     #define PWM_ARM_0_SERVO_3    5  
//     #define PWM_ARM_0_SERVO_4    6   
//     #define PWM_ARM_0_SERVO_5    7   
// #endif

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

struct ServoSpeed{
    int speedTime;
    int speedStep;
};

/************************************************************
*   class ArmServo
*   derived from Servo class, control the single servo on
*   the robotic arm. 
************************************************************/
class ArmServo:public Servo{
private:
    int _angle;
    int _speedLevel;
    struct ServoSpeed _speed;
public:
    ArmServo();
    int writeAngle(int angle);
    int getOldAngle();
    int setSpeedLevel(int speedLev);
    int getSpeedLevel();
    int setSpeed(ServoSpeed s);
    int getSpeed(ServoSpeed *s);
};

ArmServo::ArmServo()
{
    _angle = 0;
    _speedLevel = 0;
    _speed.speedTime = SPEED_LEVEL_0_DELAY_TIME;
    _speed.speedStep = SPEED_LEVEL_0_STEP;
}

int ArmServo::setSpeedLevel(int speedLev)
{
    if(speedLev >= 0 && speedLev < SPEED_LEVEL_MAX){
        _speedLevel = speedLev;
        switch(speedLev){
            case 0: _speed.speedTime = SPEED_LEVEL_0_DELAY_TIME;
                    _speed.speedStep = SPEED_LEVEL_0_STEP;
                    break;
            case 1: _speed.speedTime = SPEED_LEVEL_1_DELAY_TIME;
                    _speed.speedStep = SPEED_LEVEL_1_STEP;
                    break;
            case 2: _speed.speedTime = SPEED_LEVEL_2_DELAY_TIME;
                    _speed.speedStep = SPEED_LEVEL_2_STEP;
                    break;
            default:_speed.speedTime = SPEED_LEVEL_0_DELAY_TIME;
                    _speed.speedStep = SPEED_LEVEL_0_STEP;
                    break;
        }
    }
    else
        return -1;

    return 0;
}

int ArmServo::getSpeedLevel()
{
    return _speedLevel;
}

int ArmServo::setSpeed(ServoSpeed s)
{
    if(s.speedTime > 0 && s.speedTime < SPEED_DELAY_TIME_MAX && s.speedStep > 0 && s.speedStep < SPEED_STEP_MAX){
        _speed.speedTime = s.speedTime;
        _speed.speedStep = s.speedStep;
    }
    else
        return -1;

    return 0;
}

int ArmServo::getSpeed(ServoSpeed *s)
{
    s->speedTime = _speed.speedTime;
    s->speedStep = _speed.speedStep;
    return 0;
}

int ArmServo::writeAngle(int angle)
{
    _angle = angle;
    write(angle);

    return 0;
}

int ArmServo::getOldAngle()
{
    return _angle;
}

/************************************************************
*   class JoyArm
*   Control one single robotic arm.
************************************************************/
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
    _servo = NULL;
    _totalServoNum = 0;
}

JoyArm::~JoyArm()
{
    if(_servo != NULL)
        free(_servo);
}

int JoyArm::setServoNum(int num)
{
    if(num <= TOTAL_SERVO_NUM_MAX)
        _totalServoNum = num;
    else
        return -1;

    return 0;

}

int JoyArm::getServoNum()
{
    return _totalServoNum;
}

int JoyArm::initArmServo()
{
    if(_totalServoNum == 0)
        return -1;

    _servo = (ArmServo*)malloc(sizeof(ArmServo) * _totalServoNum);
    if(_servo == NULL)
        return -1;

    return 0;
}

//================================================================
int led = 4;
int ledErr = 5;
const int initServoValue[6] = {90, 90, 90, 90, 90, 90};
struct ServoSpeed speedSlow, speedMid, speedFast;

JoyArm *pArm = NULL;
int totalArmNum = 0;

void showError()
{
    digitalWrite(ledErr, HIGH);  // ledErr light up.
}

int initArms(char *rec_buf)
{
    totalArmNum = (int)((rec_buf[4] & 0xF0) >> 4);
    int servoNum = (int)rec_buf[5];
    if(pArm != NULL){
        free(pArm);
        pArm == NULL;
    }
    pArm = (JoyArm*)malloc(sizeof(JoyArm) * totalArmNum);
    if(pArm == NULL){
        showError();
        return -1;
    }

    // attach servo to mega2560 pwm pin
    if(totalArmNum == DOUBLE_ARM){
        pArm[0]._servo[0].attach(PWM_ARM_0_SERVO_0);
        pArm[0]._servo[1].attach(PWM_ARM_0_SERVO_1);
        pArm[0]._servo[2].attach(PWM_ARM_0_SERVO_2);
        pArm[0]._servo[3].attach(PWM_ARM_0_SERVO_3);
        pArm[0]._servo[4].attach(PWM_ARM_0_SERVO_4);
        pArm[0]._servo[5].attach(PWM_ARM_0_SERVO_5);
        pArm[1]._servo[0].attach(PWM_ARM_1_SERVO_0);
        pArm[1]._servo[1].attach(PWM_ARM_1_SERVO_1);
        pArm[1]._servo[2].attach(PWM_ARM_1_SERVO_2);
        pArm[1]._servo[3].attach(PWM_ARM_1_SERVO_3);
        pArm[1]._servo[4].attach(PWM_ARM_1_SERVO_4);
        pArm[1]._servo[5].attach(PWM_ARM_1_SERVO_5);
    }

    // initial arm and servo angle and speed
    int i = 0;
    for(i = 0; i < totalArmNum; i++){
        if(pArm[i].setServoNum(servoNum) < 0)
            showError();
        
        if(pArm[i].initArmServo() < 0)
            showError();

        int j = 0;
        for(j = 0; j < pArm[i].getServoNum(); j++){
            pArm[i]._servo[j].writeAngle(initServoValue[j]);
            pArm[i]._servo[j].setSpeedLevel(0);
        }
    }

    return 0;
}

int processSetupConnection(char *rec_buf)
{
    // digitalWrite(led, HIGH);
    if(initArms(rec_buf) < 0)
        return -1;
    
    char checksum = (char)0x00;
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
    if(servoIdx >= pArm[armIdx].getServoNum())
        return -1;
    ArmServo servo = pArm[armIdx]._servo[servoIdx];

    float angle = 0.0;
    char buf[4];
    int i;
    for(i = 0; i < 4; i++)
        buf[i] = rec_buf[i+6];
    // parse angle value from receive data
    memcpy(&angle, buf, sizeof(float));
    angle = map(angle, -90,90, 0,180);

    // get current angle value to compare with target angle value
    int oldAngle = servo.getOldAngle();

    // get current servo speed value.
    ServoSpeed s;
    servo.getSpeed(&s);
    step = s.speedstep;
    delay = s.speedtime;
    // compare and set target angle value.
    if(ABS(angle, oldAngle) > step){
        int tmpAngle = oldAngle;

        while(true){  // move to target angle in step and step
            if(angle > oldAngle){
                tmpAngle += step;
                if(tmpAngle >= angle){
                    servo.writeAngle(angle);
                    break;
                }
                else
                    servo.writeAngle(tmpAngle);
            }
            else{  // angle < oldAngle
                tmpAngle -= step;
                if(tmpAngle <= angle){
                    servo.writeAngle(angle);
                    break;
                }
                else
                    servo.writeAngle(tmpAngle);
            }
            delay(delay);
        }
    }
    else
        servo.writeAngle(angle);
    
    // send back to host.
    char checksum = (char)0;
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

    return 0;
}

int processSetSpeed(char *rec_buf)
{
    int armIdx = (int)(rec_buf[4] & 0x0F);
    if(armIdx >= totalArmNum)
        return -1;

    int servoIdx = (int)rec_buf[5];
    if(servoIdx >= pArm[armIdx].getServoNum())
        return -1;
    ArmServo servo = pArm[armIdx]._servo[servoIdx];

    ServoSpeed speed;
    int delay;
    int step;
    // char buf1[2], buf2[2];
    // buf1[0] = rec_buf[6];
    // buf1[1] = rec_buf[7];    
    // buf2[0] = rec_buf[8];
    // buf2[1] = rec_buf[9];
    // parse speed value from receive data
    memcpy(&delay, rec_buf+6, 2);
    memcpy(&step, rec_buf+8, 2);
    speed.speedTime = delay;
    speed.speedStep = step;
    if(servo.setSpeed(speed) < 0){
        showError();
        return -1;
    }

    return 0;
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
}

void loop()
{
    listenToHost();
  
    delay(10);
    digitalWrite(led, LOW);
    delay(10);
}