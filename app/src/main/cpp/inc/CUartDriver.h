//
// Created by Valpha on 2019/8/19.
//

#ifndef MYJNIDEMO_CUARTDRIVER_H
#define MYJNIDEMO_CUARTDRIVER_H

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "IComDriver.h"
#include "CPRClog.h"

#define SET_BAUD    "setbaud"

class CUartDriver : public IComDriver{
public:
    CUartDriver();
    ~CUartDriver();

public:
    virtual bool openDriver(void);
    virtual void closeDriver(void);
    virtual int sendData(const void *databuf, int length);
    virtual int receiveData(void *databuf, int length);
    virtual bool setParamater(const char* cmd, void* arg, int length);

private:
    // 串口操作函数
    bool openUart(void);
    bool setUartSpeed(int fd, int speed);
    bool setUartParity(int fd, int databits, int stopbits, int parity);

private:
    int m_uartfd;

};

#endif //MYJNIDEMO_CUARTDRIVER_H
