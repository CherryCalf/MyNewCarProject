//
// Created by hegf on 2020/4/20.
//

#ifndef MYJNIDEMO_ICOMDRIVER_H
#define MYJNIDEMO_ICOMDRIVER_H

class IComDriver{
public:
    IComDriver(){};
    virtual ~IComDriver(){};
public:
    virtual bool openDriver(void) = 0;
    virtual void closeDriver(void) = 0;
    virtual int sendData(const void *databuf, int length) = 0;
    virtual int receiveData(void *databuf, int length) = 0;
    virtual bool setParamater(const char* cmd, void* arg, int length) = 0;


};

#endif //MYJNIDEMO_ICOMDRIVER_H
