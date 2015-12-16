/*
    Copyright (C) 2015-2016,  Yanpeng Li (Robin)  lyp40293@gmail.com

    JoyArm is a project that controlling the robot 6DOF robotic arm using
    the joystick.It has two operation modes whick can control single or
    double robotic arms using one joystick. In single mode two sticks and 
    left&right RT are used to control the single robitc arm.In double mode 
    all the sticks and buttons are used to control the two robotic arms.

    protocal:
    1. setup connection: post:  AB CD E0 FF | xx(first x for total arm number, second x for this arm index) | xx(total servo number) | 12 34 56 78(test  value) BA,   
                         get:   BA DC 0E FF | xx(first x for total arm number, second x for this arm index) | xx(total servo number)| checksum AB

    2. arm write speed: post:   AB CD E0 F0 | xx(first x for total arm number, second x for this arm index) | xx(servo index value) | xx xx (speed delay time value) xx xx(speed step value) BA,   
                         get:   BA DC 0E F0 | xx(first x for total arm number, second x for this arm index) | xx(servo index value) | checksum AB

    3. arm write angle: post:   AB CD E0 0F | xx(first x for total arm number, second x for this arm index) | xx(servo index value) | xx xx xx xx(angle value) BA,
                         get:   BA DC 0E 0F | xx(first x for total arm number, second x for this arm index) | xx(servo index value) | checksum AB

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

#include "joyarm.h"


JoyArm::JoyArm(uint8_t const total_arm_num, uint8_t const arm_index)
{
    if(total_arm_num > TOTAL_ARM_MAX){
        cout << "The max total arm number should be " << TOTAL_ARM_MAX << "." << endl;
        _total_arm_num = TOTAL_ARM_MAX;
    }
    else
        _total_arm_num = total_arm_num;

    if(arm_index > TOTAL_ARM_MAX - 1){
        cout << "The arm index exceed the total arm number." << endl;
        _arm_index = TOTAL_ARM_MAX - 1;
    }
    else
        _arm_index = arm_index;

    _arm_servo_num = SERVO_NUM;
    _servos = (struct Servo*)malloc(sizeof(struct Servo) * _arm_servo_num);
    if(_servos != NULL){
        for (int i = 0; i < _arm_servo_num; i++){
            _servos[i].idx = i;
            _servos[i].speed.speed_time = 30;
            _servos[i].speed.speed_step = 1;
            _servos[i].angle = 0.0;   // range from -90 degree to 90 degree
            _servos[i].max_angle = 90.0;
            _servos[i].min_angle = -90.0;
        }
        _com.set_opt(9600, 8, 'N',1);
    }
}

JoyArm::JoyArm(uint8_t const total_arm_num, uint8_t const arm_index, uint8_t const servo_num)
{
    if(total_arm_num > TOTAL_ARM_MAX){
        cout << "The max total arm number should be " << TOTAL_ARM_MAX << "." << endl;
        _total_arm_num = TOTAL_ARM_MAX;
    }
    else
        _total_arm_num = total_arm_num;

    if(arm_index > TOTAL_ARM_MAX - 1){
        cout << "The arm index exceed the total arm number." << endl;
        _arm_index = TOTAL_ARM_MAX - 1;
    }
    else
        _arm_index = arm_index;

    if(servo_num > SERVO_NUM_MAX){
        cout << "The servo number in single arm exceed the max servo number." << endl;
        _total_arm_num = SERVO_NUM_MAX;
    }
    else 
        _total_arm_num = servo_num;

    _servos = (struct Servo*)malloc(sizeof(struct Servo) * _arm_servo_num);
    if(_servos != NULL){
        for (int i = 0; i < _arm_servo_num; i++){
            _servos[i].idx = i;
            _servos[i].speed.speed_time = 30;
            _servos[i].speed.speed_step = 1;
            _servos[i].angle = 0.0;   // range from -90 degree to 90 degree
            _servos[i].max_angle = 90.0;
            _servos[i].min_angle = -90.0;
        }
        _com.set_opt(9600, 8, 'N',1);
    }
}

JoyArm::~JoyArm()
{
    if(_servos != NULL)
        free(_servos);
}

int JoyArm::checkout_data(char *send_buf, int send_len, char checksum)
{
    if(send_buf == NULL){
        cout << "Invalid input send_buf." << endl;
        return -1;
    }

    int rec_len = 50;
    char rec_buf[rec_len];

    _com.com_read(rec_buf, rec_len);

    int i = 0; 
    while(true){
        if(i >= rec_len - send_len){
            //cout << "invalid received data." << endl;
            return CHECK_FAILED;
        }

        // check the received data.
        // the right data array may not be from the start of the received data array.
        if (rec_buf[i] == (char)0xBA && rec_buf[i+1] == (char)0xDC && rec_buf[i+2] == (char)0x0E)
            if ( rec_buf[i+3] == (char)0xFF && rec_buf[i+4] == send_buf[4]  && rec_buf[i+5] == send_buf[5] && rec_buf[i+6] == checksum && rec_buf[i+7] == (char)0xAB){
                cout << "Connect to JoyArm " << (int)_arm_index << " successfully!" << endl;
                return CHECK_CONNECTION_SETUP;
            } 
            else if (rec_buf[i+3] == (char)0xF0 && rec_buf[i+4] == send_buf[4]  && rec_buf[i+5] == send_buf[5] && rec_buf[i+6] == checksum && rec_buf[i+7] == (char)0xAB){
                char buf[4];
                for(int i = 0; i < 4; i++)
                    buf[i] = send_buf[i+6];
                float speed = 0.0;
                memcpy(&speed, buf, sizeof(float));
                cout << "Set JoyArm " << (int)_arm_index << " servo " << (int)send_buf[5] << " speed to " << speed << "." << endl;
                return CHECK_SET_SPEED;
            }
            else if (rec_buf[i+3] == (char)0x0F && rec_buf[i+4] == send_buf[4]  && rec_buf[i+5] == send_buf[5] && rec_buf[i+6] == checksum && rec_buf[i+7] == (char)0xAB){
                char buf[4];
                for(int i = 0; i < 4; i++)
                    buf[i] = send_buf[i+6];
                float angle = 0.0;
                memcpy(&angle, buf, sizeof(float));
                cout << "Set JoyArm " << (int)_arm_index << " servo " << (int)send_buf[5] << " angle to " << angle << "." << endl;
                return CHECK_SET_ANGLE;
            }
            else {
                i += 3;
            }
        else{
            i += 3;
        }

    }
}

int JoyArm::setup_com_connection()
{
    char send_buf[11];
    send_buf[0] = (char)0xAB;
    send_buf[1] = (char)0xCD;
    send_buf[2] = (char)0xE0;
    send_buf[3] = (char)0xFF;
    send_buf[4] = (char)(_total_arm_num << 4 + _arm_index);
    send_buf[5] = (char)(_arm_servo_num);
    send_buf[6] = (char)0x12;
    send_buf[7] = (char)0x34;
    send_buf[8] = (char)0x56;
    send_buf[9] = (char)0x78;
    send_buf[10] = (char)0xBA;

    int timeout = 10; // 10 seconds timeout.
    int send_len = sizeof(send_buf);
    _com.com_write(send_buf, send_len);
    cout << "Waiting for connection......" << endl;

    char checksum = (char)0;
    for(int i = 0; i < send_len; i++)
        checksum += send_buf[i];


    while(timeout){
        // need to write several times to ensure the right data sent to the Arduino.
        _com.com_write(send_buf, send_len);

        int ret = checkout_data(send_buf, send_len, checksum);

        if( ret == CHECK_CONNECTION_SETUP)
            return 0;
        else {
            timeout--;
            sleep(1);
            continue;
        }
    }

    if(timeout == 0){
        cout << "Connect to JoyArm " << (int)_arm_index << " overtime." << endl;
        return -1;
    }

    return 0;
}

int JoyArm::set_servo_num(uint8_t num)
{
    if (num > SERVO_NUM_MAX){
        cout << "Servo number is beyond the max." << endl;
        return -1;
    }

    _arm_servo_num = num;

    if(_servos != NULL){
        free(_servos);
        _servos = NULL;
    }

    _servos = (struct Servo*)malloc(sizeof(struct Servo) * _arm_servo_num);
    if(_servos != NULL){
        for (int i = 0; i < _arm_servo_num; i++){
            _servos[i].idx = i;
            _servos[i].speed.speed_time = 30;
            _servos[i].speed.speed_step = 1;
            _servos[i].angle = 0.0;   // range from -90 degree to 90 degree
            _servos[i].max_angle = 90.0;
            _servos[i].min_angle = -90.0;
        }
    }
    else{
        cout << "malloc space for _servos failed." << endl;
        return -1;
    }

    return 0;
}

uint8_t JoyArm::get_servo_num()
{
    return _arm_servo_num;
}

// set certain servo angle with current speed
int JoyArm::set_servo_angle(uint8_t idx, float angle)
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
    char buf[4];
    memcpy(buf, &angle, sizeof(float));

    char send_buf[11];
    send_buf[0] = (char)0xAB;
    send_buf[1] = (char)0xCD;
    send_buf[2] = (char)0xE0;
    send_buf[3] = (char)0x0F;
    send_buf[4] = (char)(_total_arm_num << 4 + _arm_index);
    send_buf[5] = (char)idx;
    for(int i = 0; i < 4; i++)
        send_buf[i+6] = buf[i];
    send_buf[10] = (char)0xBA;

    int timeout = 10; // 10 seconds timeout.
    int send_len = sizeof(send_buf);

    char checksum = (char)0;
    for(int i = 0; i < send_len; i++)
        checksum += send_buf[i];

    while(timeout){
        // need to write several times to ensure the right data sent to the Arduino.
        _com.com_write(send_buf, send_len);

        int ret = checkout_data(send_buf, send_len, checksum);

        if( ret == CHECK_SET_ANGLE)
            return 0;
        else {
            timeout--;
            msleep(30);
            continue;
        }
        msleep(30);
    }

    if(timeout == 0){
        cout << "Set JoyArm angle failed." << endl;
        return -1;
    }

    return 0;
}

// set certain servo speed
int JoyArm::set_servo_speed(uint8_t idx, ServoSpeed speed)
{
    if (idx > _arm_servo_num){
        cout << "invalid servo index." << endl;
        return -1;
    }
    if (speed.speed_time < 0 || speed.speed_time > SERVO_SPEED_DELAY_TIME_MAX || speed.speed_step < 0 || speed.speed_step > SERVO_SPEED_STEP_MAX){
        cout << "invalid servo speed." << endl;
        return -1;
    }

    int delay = speed.speed_time;
    int step = speed.speed_step;
    _servos[idx].speed.speed_time = delay;
    _servos[idx].speed.speed_step = step;

    char buf1[2], buf2[2];
    memcpy(buf1, &delay, 2);
    memcpy(buf2, &step, 2);

    char send_buf[11];
    send_buf[0] = (char)0xAB;
    send_buf[1] = (char)0xCD;
    send_buf[2] = (char)0xE0;
    send_buf[3] = (char)0xF0;
    send_buf[4] = (char)(_total_arm_num << 4 + _arm_index);
    send_buf[5] = (char)idx;
    send_buf[6] = buf1[0];
    send_buf[7] = buf1[1];
    send_buf[8] = buf2[0];
    send_buf[9] = buf2[1];
    send_buf[10] = (char)0xBA;

    int timeout = 10; // 10 seconds timeout.
    int send_len = sizeof(send_buf);

    char checksum = (char)0;
    for(int i = 0; i < send_len; i++)
        checksum += send_buf[i];

    while(timeout){
        // need to write several times to ensure the right data sent to the Arduino.
        _com.com_write(send_buf, send_len);

        int ret = checkout_data(send_buf, send_len, checksum);

        if( ret == CHECK_SET_SPEED)
            return 0;
        else {
            timeout--;
            sleep(1);
            continue;
        }
    }

    if(timeout == 0){
        cout << "Set JoyArm speed failed." << endl;
        return -1;
    }
    return 0;
}


int JoyArm::set_servo_angle_speed(uint8_t idx, float angle, ServoSpeed speed)
{
    if(set_servo_angle(idx, angle))
        return -1;

    if(set_servo_speed(idx, speed))
        return -1;

    return 0;
}

int JoyArm::get_servo_speed(uint8_t idx, ServoSpeed *speed)
{
    if (idx > _arm_servo_num){
        cout << "invalid servo index." << endl;
        return -1;
    }

    if (speed == NULL){
        cout << "invalid speed pointer." << endl;
        return -1;
    }

    speed->speed_time = _servos[idx].speed.speed_time;
    speed->speed_step = _servos[idx].speed.speed_step;

    return 0;
}

int JoyArm::get_servo_angle(uint8_t idx, float *angle)
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


