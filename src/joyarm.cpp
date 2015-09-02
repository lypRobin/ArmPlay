/*
    Copyright (C) <2015>  <Robin:lyp40293@gmail.com>

    JoyArm is a project that controlling the robot 6DOF robotic arm using
    the joystick.It has two operation modes whick can control single or
    double robotic arms using one joystick. In single mode two sticks and 
    left&right RT are used to control the single robitc arm.In double mode 
    all the sticks and buttons are used to control the two robotic arms.

    protocal:
    1. setup connection: post:  AB CD E0 01 BA,   get: BA 12 E0 CD AB,  connection setup!
    2. single arm write speed: post:        AB CD E0 C0 10 xx xx xx xx xx xx xx xx(speed value) BA,   
    3. single arm write angle: post:        AB CD E0 C0 01 xx xx xx xx xx xx xx xx(angle value) BA,
    4. double arm write left speed: post:   AB CD E0 D0 10 xx xx xx xx xx xx xx xx(speed value) BA, 
    5. double arm write left angle: post:   AB CD E0 D0 01 xx xx xx xx xx xx xx xx(angle value) BA, 
    5. double arm write right speed: post:  AB CD E0 0D 10 xx xx xx xx xx xx xx xx(speed value) BA, 
    6. double arm write right angle: post:  AB CD E0 0D 01 xx xx xx xx xx xx xx xx(angle value) BA, 

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

#include "joystick.h"
#include "com.h"

#define JOYSTICK_DEV "/dev/input/js0"
#define JS_BUTTON 1
#define JS_AXIS 2

#define SERVO_NUM 6
#define SERVO_NUM_MAX  20
#define SERVO_SPEED_MAX 100.0  // 100%
#define SERVO_SPEED_MIN 1.0  // 1.0%

using namespace std;
typedef unsigned char _8u;

struct Servo{
    _8u idx;
    float angle;
    float speed;
    float max_angle;
    float min_angle;
};

class JoyArm{
public: 
    JoyArm();
    JoyArm(_8u const servo_num);
    ~JoyArm();

private:
    int _arm_servo_num;
    Servo *_servos;
    Com _com;

public:
    int set_servo_num(_8u num);
    _8u get_servo_num();
    int set_servo_speed(_8u idx, float speed);
    int get_servo_speed(_8u idx, float *speed);
    int set_servo_angle(_8u idx, float angle);
    int get_servo_angle(_8u idx, float *angle);
    int setup_com_connection();
    //void compute_hand_pos();  // for detail
    //void compute_hand_vet();  // for detail
    
};

JoyArm::JoyArm()
{
    _arm_servo_num = SERVO_NUM;
    _servos = (struct Servo*)malloc(sizeof(struct Servo) * _arm_servo_num);
    for (int i = 0; i < _arm_servo_num; i++){
        _servos[i].idx = i;
        _servos[i].speed = 0.0;
        _servos[i].angle = 0.0;
        _servos[i].max_angle = 180.0;
        _servos[i].min_angle = 0.0;
    }
    _com.set_opt(9600, 8, 'N',1);
}

JoyArm::JoyArm(_8u const servo_num)
{
    _arm_servo_num = servo_num;
    _servos = (struct Servo*)malloc(sizeof(struct Servo) * _arm_servo_num);
    for (int i = 0; i < _arm_servo_num; i++){
        _servos[i].idx = i;
        _servos[i].speed = 0.0;
        _servos[i].angle = 0.0;
        _servos[i].max_angle = 180.0;
        _servos[i].min_angle = 0.0;
    }
}

JoyArm::~JoyArm()
{
    free(_servos);
}

int JoyArm::setup_com_connection()
{
    _8u send_buf[5] = {0xAB, 0xCD, 0xE0, 0x12, 0xBA};

    bool is_connect = false;
    int timeout = 10; // 10 seconds timeout.
    int send_len = sizeof(send_buf) / sizeof(_8u);
    _com.com_write(send_buf, send_len);
    cout << "Waiting for connection......" << endl;

    while(timeout){
        // need to write several times to ensure the right data sent to the Arduino.
        _com.com_write(send_buf, send_len);

        int rec_len = 50;
        _8u rec_buf[rec_len];

        _com.com_read(rec_buf, rec_len);
        int i = 0; 
        while(true){
            if(i == rec_len - send_len){
                //cout << "invalid received data." << endl;
                break;
            }

            // check the received data.
            // the right data array may not be from the start of the received data array.
            if (rec_buf[i] == (_8u)0xBA)
                if (rec_buf[i+1] == (_8u)0x12 && rec_buf[i+2] == (_8u)0xE0 && rec_buf[i+3] == (_8u)0xCD && rec_buf[i+4] == (_8u)0xAB){
                    cout << "Connect to JoyArm successfully!" << endl;
                    return 0;
                }
                else{
                    i++;
                }
            else{
                i++;
            }

        }

        timeout--;
        sleep(1);
    }

    if(timeout == 0){
        cout << "Connect to serial port failed." << endl;
        return -1;
    }

    return 0;
}

int JoyArm::set_servo_num(_8u num)
{
    if (num > SERVO_NUM_MAX){
        cout << "Servo number is beyond the max." << endl;
        return -1;
    }

    _arm_servo_num = num;
    free(_servos);
    _servos = NULL;
    _servos = (struct Servo*)malloc(sizeof(struct Servo) * _arm_servo_num);
    for (int i = 0; i < _arm_servo_num; i++){
        _servos[i].idx = i;
        _servos[i].speed = 0.0;
        _servos[i].angle = 0.0;
        _servos[i].max_angle = 180.0;
        _servos[i].min_angle = 0.0;
    }

    return 0;
}

_8u JoyArm::get_servo_num()
{
    return _arm_servo_num;
}

int JoyArm::set_servo_angle(_8u idx, float angle)
{
    if (idx > _arm_servo_num){
        cout << "invalid servo index." << endl;
        return -1;
    }
    if (angle > _servos[idx].max_angle || angle < _servos[idx].min_angle){
        cout << "invalid servo angle." << endl;
        return -1;
    }

    _servos[idx].angle = angle;

    _8u buf[10];
    stringstream ss;
    ss << angle;
    ss >> buf;
    ss.clear();

    _com.com_write((_8u*)buf, sizeof(buf) / sizeof(_8u));

    return 0;
}

int JoyArm::set_servo_speed(_8u idx, float speed)
{
    if (idx > _arm_servo_num){
        cout << "invalid servo index." << endl;
        return -1;
    }
    if (speed > SERVO_SPEED_MAX || speed < SERVO_SPEED_MIN){
        cout << "invalid servo speed." << endl;
        return -1;
    }

    _servos[idx].speed = speed;

    _8u buf[10];
    stringstream ss;
    ss << speed;
    ss >> buf;
    ss.clear();

    _com.com_write((_8u*)buf, sizeof(buf) / sizeof(_8u));

    return 0;
}

int JoyArm::get_servo_speed(_8u idx, float *speed)
{
    if (idx > _arm_servo_num){
        cout << "invalid servo index." << endl;
        return -1;
    }

    if (speed == NULL){
        cout << "invalid speed pointer." << endl;
        return -1;
    }

    *speed = _servos[idx].speed;

    return 0;
}

int JoyArm::get_servo_angle(_8u idx, float *angle)
{
    if (idx > _arm_servo_num){
        cout << "invalid servo index." << endl;
        return -1;
    }

    if (angle == NULL){
        cout << "invalid angle pointer." << endl;
        return -1;
    }

    *angle = _servos[idx].angle;

    return 0;
}

// Main
int main(int argc, char* argv[])
{
    JoyStick js(JOYSTICK_DEV);
    int axis_num = js.get_axis_number();
    int button_num = js.get_button_number();

    JoyArm left_arm;
    JoyArm right_arm;

    left_arm.setup_com_connection();
    right_arm.setup_com_connection();





    stringstream ss;
        _8u type = 0;
        _8u idx = 0;
        int val = 0;
    while (true){

        if (js.read_joystick(&type, &idx, &val) < 0){
            cout << "read joystick error!" << endl;
            continue;
        }
        else{
            if (type == JS_AXIS){
                cout << "Axis " << (int)idx << ": " << val << ";" << endl;
            }
            else if (type == JS_BUTTON){
                cout << "Button " << (int)idx << ": " << val << ";" << endl;
            }
            else
                continue;

        }
    }

    return 0;
}