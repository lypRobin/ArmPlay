/*
    Copyright (C) <2015>  <Robin:lyp40293@gmail.com>

    JoyArm is a project that controlling the robot 6DOF robotic arm using
    the joystick.It has two operation modes whick can control single or
    double robotic arms using one joystick. In single mode two sticks and 
    left&right RT are used to control the single robitc arm.In double mode 
    all the sticks and buttons are used to control the two robotic arms.

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

#define JOYSTICK_DEV "/dev/input/js0"
#define JS_BUTTON 1
#define JS_AXIS 2

using namespace std;


int main()
{
    JoyStick js(JOYSTICK_DEV);
    int axis_num = js.get_axis_number();
    int button_num = js.get_button_number();

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