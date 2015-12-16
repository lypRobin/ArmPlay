/*
    Copyright (C) 2015-2016,  Yanpeng Li (Robin)  lyp40293@gmail.com

    Com class present the communication between Arduino and ths host PC /dev/ttyACM0.

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

#ifndef _COM_H_
#define _COM_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sstream>

using namespace std;

typedef unsigned char uint8_t;

class Com{
public: 
    Com();
    Com(char *com_dev);
    ~Com();

private:
    string _com_dev;  // com dev name
    int _fd;   // com file descriptor

public:
    int set_opt(int speed, int bits, char event, int stop);
    int com_read(char *buf, int len);
    int com_write(char  *buf, int len);

};

#endif