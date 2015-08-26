/*
    Copyright (C) <2015>  <Robin:lyp40293@gmail.com>

    JoyArm is a project that controlling the 6DOF robot arm using joysitck.

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


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <linux/joystick.h>

using namespace std;

#define JOYSTICK_DEV "/dev/input/js0"

class JoyStick{
public:
    JoyStick();
    ~JoyStick();

private:
    string _joystick_id;
    int _fd;
    int _axis_number;
    int _button_number;
    struct js_event _jsevent;

public:
    int open_dev();
    const string get_joystick_name();
    int get_axis_number();
    int get_button_number();
    int get_axis_value();
    int get_button_value();
    void close_dev();
    void run();
};


// 
JoyStick::JoyStick()
{
    _axis_number = 0;
    _button_number = 0;
    _fd = open_dev();

}

JoyStick::~JoyStick()
{
    this->close_dev();
}

int JoyStick::open_dev()
{
    int fd = open(JOYSTICK_DEV, O_RDONLY);
    if(fd < 0){
        cout << "Open device: '" << JOYSTICK_DEV << "' failed, please check device connection!" << endl;
        return -1;
    }
    else{
        char axis;
        char buttons;
        char name[128];
        if (ioctl(fd, JSIOCGNAME(sizeof(name)), name) < 0)
            strncpy(name, "Unknown", sizeof(name));
        _joystick_id = (string)name;

        ioctl(fd, JSIOCGAXES, &axis);
        _axis_number = (int)axis;

        ioctl(fd, JSIOCGBUTTONS, &buttons);
        _button_number = (int)buttons;
        cout << "Open joystick: '" << JOYSTICK_DEV << "' successfully, device ID is: " << _joystick_id << "." << endl;

        return fd;
    }
}

void JoyStick::close_dev()
{
    close(_fd);
}

int JoyStick::get_axis_number()
{
    return _axis_number;
}

int JoyStick::get_button_number()
{
    return _button_number;
}

const string JoyStick::get_joystick_name()
{
    return _joystick_id;
}

void JoyStick::run()
{
    while(true){
        struct js_event je;
        if (read(_fd, &je, sizeof(js_event)) < 0){
            cout << "Read JoyStick failed!" << endl;
            continue;
        }

        je.type &= ~JS_EVENT_INIT; 

        switch(je.type){
            case JS_EVENT_BUTTON:
                cout << "Button: " << (int)je.number << " value is: " << je.value << "." << endl;
                break;
            case JS_EVENT_AXIS:
                cout << "AXIS: " << (int)je.number << " value is: " << je.value << "." << endl;
                break;
            default:
                break;
        }
    }
}

int main()
{
    JoyStick js;
    std::cout << "The joystick name is: " << js.get_joystick_name() << "." << std::endl;
    std::cout << "The number of joystick buttons is: " << js.get_button_number() << "." << std::endl;
    std::cout << "The number of joystick axis is: " << js.get_axis_number() << "." << std::endl;

    js.run();
    return 0;
}