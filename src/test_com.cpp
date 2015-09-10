/*
    Copyright (C) <2015>  by Robin  lyp40293@gmail.com

    Test the communication between Arduino and ths host PC /dev/ttyACMx
    using the Com class.

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

#include "com.h"
using namespace std;


int main()
{
    Com com;
    com.set_opt(9600, 8, 'N', 1);
    char send_buf[5] = {0xAB, 0xCD, 0xE0, 0x12, 0xBA};

    bool is_connect = false;
    int timeout = 10; // 10 seconds timeout.
    int send_len = sizeof(send_buf) / sizeof(_8u);
    com.com_write(send_buf, send_len);
    cout << "Waiting for connection......" << endl;

    while(timeout){
        if(is_connect)
            break;
        // need to write several times to ensure the right data sent to the Arduino.
        com.com_write(send_buf, send_len);

        int rec_len = 50;
        char rec_buf[rec_len];

        com.com_read(rec_buf, rec_len);
        char checksum = (char)(0xAB + 0xCD + 0xE0 + 0x12 + 0xBA);
        int i = 0; 
        while(true){
            if(i == rec_len - send_len){
                //cout << "invalid received data." << endl;
                break;
            }

            // check the received data.
            // the right data array may not be from the start of the received data array.
            if (rec_buf[i] == (char)0xBA)
                if (rec_buf[i+1] == (char)0x12 && rec_buf[i+2] == (char)0xE0 && rec_buf[i+3] == (char)0xCD && rec_buf[i+4] == (char)0xAB && rec_buf[i+5] == (char)checksum){
                    cout << "connection setup!!!!!!!!!!!!!!!!!!" << endl;
                    is_connect = true;
                    break;
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
    }

    return 0;
}