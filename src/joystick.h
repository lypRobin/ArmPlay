/*
    Copyright (C) 2015-2016,  Yanpeng Li (Robin)  lyp40293@gmail.com

    joystick.h is the header of a joystick class.

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

#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

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
#include <sstream>
#include <linux/joystick.h>

using namespace std;

typedef unsigned char uint8_t;

class JoyStick{
public:
    JoyStick(const char *dir);
    ~JoyStick();

private:
    string _joystick_name;
    int _fd;
    int _axis_number;
    int _button_number;
    struct js_event _jsevent;

public:
    int open_dev(const char* dir);
    int read_joystick(uint8_t *type, uint8_t *idx, int *val);
    const string get_joystick_name();
    int get_axes_number();
    int get_button_number();
    void close_dev();
};

#endif
