/*
    Copyright (C) <2015>  by Robin  lyp40293@gmail.com

    Test joystick.

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

int main()
{
    JoyStick js("/dev/input/js0");

    const string js_name = js.get_joystick_name();
    int axes_num = js.get_axes_number();
    int btn_num = js.get_button_number();
    cout << "Open Joystick " << js_name << "." << endl;
    cout << "Axes number is " << axes_num << "." << endl;
    cout << "Button number is " << btn_num << "." << endl;

    _8u type = 0;
    _8u idx = 0;
    int value = 0;

    while(true){
        js.read_joystick(&type, &idx, &value);
    }
    return 0;
}







