/*
    Copyright (C) <2015>  <Robin:lyp40293@gmail.com>

    com.cpp presents the com class.

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

#define COM_ACM_DEV "/dev/ttyACM"
#define COM_USB_DEV "/dev/ttyUSB"
#define MAX_SERIAL_NUM  5
using namespace std;

Com::Com()
{
    stringstream ss;
    int i;
    for(i = 0; i < MAX_SERIAL_NUM; i++){
        ss << COM_ACM_DEV;
        ss << i;
        ss >> _com_dev;
        _fd = open(_com_dev .c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
        if( _fd < 0){
            ss.clear();
            continue;
        }
        else{
            ss >> _com_dev;
            ss.clear();
            cout << "Open serial port:" << _com_dev.c_str() << " successfully!" << endl;
            return;
        }
    }

    for(i = 0; i < MAX_SERIAL_NUM; i++){
        ss << COM_USB_DEV;
        ss << i;
        ss >> _com_dev;
        _fd = open(_com_dev .c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
        if( _fd < 0){
            ss.clear();
            continue;
        }
        else{
            ss >> _com_dev;
            ss.clear();
            cout << "Open serial port:" << _com_dev.c_str() << " successfully!" << endl;
            return;
        }
    }
    
    if(i == MAX_SERIAL_NUM)
        cout << "Open serial port failed." << endl;

}

Com::Com(char* com_dev)
{
    _com_dev = com_dev;
    _fd = open(_com_dev.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if( _fd < 0)
        cout << "Open serial port:" << _com_dev.c_str() << " failed." << endl;
    else
        cout << "Open serial port:" << _com_dev.c_str() << " successfully!" << endl;
}

Com::~Com()
{
    if(_fd >= 0){
        if (close(_fd) < 0)
            cout << "Close serial port: " << _com_dev.c_str() << " failed." << endl;
        else
            cout << "Close serial port:" << _com_dev.c_str() << "!" << endl;
    }
}

int Com::set_opt(int nSpeed, int nBits, char nEvent, int nStop)
{
    struct termios newtio,oldtio;
    if  ( tcgetattr( _fd,&oldtio)  !=  0) 
    { 
        perror("SetupSerial 1");
        return -1;
    }
    bzero( &newtio, sizeof( newtio ) );
    newtio.c_cflag  |=  CLOCAL | CREAD; 
    newtio.c_cflag &= ~CSIZE; 

    // newtio.c_lflag &= ~(ICANON | ECHO |ECHOE | ISIG);
    // newtio.c_oflag &= ~OPOST;

    // 8bits or 7bits
    switch( nBits )
    {
    case 7:
        newtio.c_cflag |= CS7;
        break;
    case 8:
        newtio.c_cflag |= CS8;
        break;
    }

    // event bit
    switch( nEvent )
    {
    case 'O':                     //奇校验
        newtio.c_cflag |= PARENB;
        newtio.c_cflag |= PARODD;
        newtio.c_iflag |= (INPCK | ISTRIP);
        break;
    case 'E':                     //偶校验
        newtio.c_iflag |= (INPCK | ISTRIP);
        newtio.c_cflag |= PARENB;
        newtio.c_cflag &= ~PARODD;
        break;
    case 'N':                    //无校验
        newtio.c_cflag &= ~PARENB;
        break;
    }

    // baudrate
    switch( nSpeed )
    {
    case 2400:
        cfsetispeed(&newtio, B2400);
        cfsetospeed(&newtio, B2400);
        break;
    case 4800:
        cfsetispeed(&newtio, B4800);
        cfsetospeed(&newtio, B4800);
        break;
    case 9600:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    case 115200:
        cfsetispeed(&newtio, B115200);
        cfsetospeed(&newtio, B115200);
        break;
    default:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    }

    // stop bit
    if( nStop == 1 )
    {
        newtio.c_cflag &=  ~CSTOPB;
    }
    else if ( nStop == 2 )
    {
        newtio.c_cflag |=  CSTOPB;
    }
    newtio.c_cc[VTIME]  = 0;
    newtio.c_cc[VMIN] = 0;
    tcflush(_fd,TCIFLUSH);

    if((tcsetattr(_fd,TCSANOW,&newtio))!=0)
    {
        perror("com set error");
        return -1;
    }
    cout << "Set serial port done!" << endl;
    return 0;
}


int Com::com_read(_8u *buf, int len)
{
    if(read(_fd, buf, len) < 0){
        cout << "read serial port failed." << endl;
        return -1;
    }

    return 0;
}

int Com::com_write(_8u *buf, int len)
{
    if(write(_fd, buf, len) < 0){
        cout << "write serial port failed." << endl;
        return -1;
    }

    return 0;
}

















 