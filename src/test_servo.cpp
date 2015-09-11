/*
    Copyright (C) <2015>  by Robin  lyp40293@gmail.com

    Test the arm with a single servo. Try different speed and angle.

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

using namespace std;

int main(int argc, char** argv)
{
    JoyArm arm(1,0,1);
    arm.setup_com_connection();
    // arm.set_servo_angle(0, 75);
    
    for (float d = -90; d < 90; d+=2)
    {
        arm.set_servo_angle(0, d);
        msleep(30);
    }

    return 0;
}