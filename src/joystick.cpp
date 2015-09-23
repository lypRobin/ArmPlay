/*
    Copyright (C) <2015>  by Robin  lyp40293@gmail.com

    joystick.cpp present a joystick class using default joystick driver.

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

using namespace std;
// initial
JoyStick::JoyStick(const char *dir)
{
    _joystick_name = "";
    _axis_number = 0;
    _button_number = 0;
    _fd = open_dev(dir);

}

JoyStick::~JoyStick()
{
    this->close_dev();
}

// open joystick dev: /dev/input/js0
int JoyStick::open_dev(const char *dir)
{
    int fd = open(dir, O_RDONLY);
    if(fd < 0){
        cout << "Open device: '" << dir << "' failed, please check device connection!" << endl;
        return -1;
    }
    else{
        char axis;
        char buttons;
        char name[128];

        // get joystick name
        if (ioctl(fd, JSIOCGNAME(sizeof(name)), name) < 0)
            strncpy(name, "Unknown", sizeof(name));
        _joystick_name = (string)name;

        // get joystick number of axises
        ioctl(fd, JSIOCGAXES, &axis);
        _axis_number = (int)axis;

        // get joystick number of buttons
        ioctl(fd, JSIOCGBUTTONS, &buttons);
        _button_number = (int)buttons;

        cout << "Open joystick: '" << dir << "' successfully, device ID is: " << _joystick_name << "." << endl;

        return fd;
    }
}

void JoyStick::close_dev()
{
    close(_fd);
}

int JoyStick::get_axes_number()
{
    return _axis_number;
}

int JoyStick::get_button_number()
{
    return _button_number;
}


const string JoyStick::get_joystick_name()
{
    return _joystick_name;
}

int JoyStick::read_joystick(_8u *type, _8u *idx, int *val)
{
    // read js0
    if (read(this->_fd, &this->_jsevent, sizeof(js_event)) < 0){
        cout << "Read JoyStick failed!" << endl;
        return -1;
    }

    // process the js event
    _jsevent.type &= ~JS_EVENT_INIT; 

    switch(_jsevent.type){
    case JS_EVENT_BUTTON:
        *type = 1;
        *idx = (int)_jsevent.number;
        *val = _jsevent.value;
        cout << "Button: " << (int)_jsevent.number << " value is: " << _jsevent.value << "." << endl;
        break;
    case JS_EVENT_AXIS:
        *type = 2;  
        *idx = (int)_jsevent.number;
        *val = _jsevent.value;    
        cout << "AXIS: " << (int)_jsevent.number << " value is: " << _jsevent.value << "." << endl;
        break;
    default:
        *type = 0;
        break;
    }

    return 0;
}


