/*
    Copyright (C) <2015>  by Robin  lyp40293@gmail.com

    JoyArm is a project that controlling the robot 6DOF robotic arm using
    the joystick.It has two operation modes whick can control single or
    double robotic arms using one joystick. In single mode two sticks and 
    left&right RT are used to control the single robitc arm.In double mode 
    all the sticks and buttons are used to control the two robotic arms.

    protocal:
    1. setup connection: post:  AB CD E0 01 BA,   get: BA 12 E0 CD AB,  connection setup!
    2. single arm write speed: post:        AB CD E0 C0 F0 | xx(index value) | xx xx xx xx xx xx xx xx(speed value) BA,   
    3. single arm write angle: post:        AB CD E0 C0 0F | xx(index value) | xx xx xx xx xx xx xx xx(angle value) BA,
    4. double arm write left speed: post:   AB CD E0 D0 F0 | xx(index value) | xx xx xx xx xx xx xx xx(speed value) BA, 
    5. double arm write left angle: post:   AB CD E0 D0 0F | xx(index value) | xx xx xx xx xx xx xx xx(angle value) BA, 
    5. double arm write right speed: post:  AB CD E0 0D F0 | xx(index value) | xx xx xx xx xx xx xx xx(speed value) BA, 
    6. double arm write right angle: post:  AB CD E0 0D 0F | xx(index value) | xx xx xx xx xx xx xx xx(angle value) BA, 

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

#ifndef _JOYARM_H_
#define _JOYARM_H_

#include "joystick.h"
#include "com.h"

#define JOYSTICK_DEV        "/dev/input/js0"
#define JS_BUTTON           1
#define JS_AXIS             2

#define TOTAL_ARM_MAX  4

#define SERVO_NUM 6
#define SERVO_NUM_MAX  20
#define SERVO_SPEED_LEVEL_MAX  3
#define SERVO_SPEED_DELAY_TIME_MAX 30
#define SERVO_SPEED_STEP_MAX 5

#define CHECK_FAILED            -1
#define CHECK_CONNECTION_SETUP  1
#define CHECK_SET_ANGLE         2
#define CHECK_SET_SPEED         3

#define msleep(x) usleep(1000*x)

using namespace std;
typedef unsigned char _8u;

struct ServoSpeed{
    int speed_time;
    int speed_step;
};

struct Servo{
    _8u idx;
    float angle;
    struct ServoSpeed speed;
    float max_angle;
    float min_angle;
};

class JoyArm{
public: 
    JoyArm(_8u const total_arm_num, _8u const arm_index);
    JoyArm(_8u const total_arm_num, _8u const arm_index, _8u const servo_num);  
    ~JoyArm();

private:
    int _arm_servo_num;
    Servo *_servos;    // each arm may have several servos
    Com _com;
    _8u _total_arm_num;
    _8u _arm_index;

public:
    int set_servo_num(_8u num);
    _8u get_servo_num();
    int set_servo_speed(_8u idx, ServoSpeed speed);
    int get_servo_speed(_8u idx, ServoSpeed *speed);
    int set_servo_angle(_8u idx, float angle);
    int get_servo_angle(_8u idx, float *angle);
    int set_servo_angle_speed(_8u idx, float angle, ServoSpeed speed);
    int setup_com_connection();
    int checkout_data(char *send_buf, int send_len, char checksum);
    //void compute_hand_pos();  // for detail
    //void compute_hand_vet();  // for detail
    
};

#endif