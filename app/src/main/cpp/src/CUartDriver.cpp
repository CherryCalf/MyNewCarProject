//
// Created by Valpha on 2019/8/19.
//


#include "../inc/CUartDriver.h"

#define UART_RPCLENGTH  16
#define UART_BAUDRATE   115200
#define UART_DEVICE     "/dev/ttymxc2"
#define UART_DATABITS   8
#define UART_STOPBITS   1
#define UART_PARITY     'N'


CUartDriver::CUartDriver() {
    m_uartfd = -1;
    LOGD("New UartDriver!\n");
}

CUartDriver::~CUartDriver() {

}

bool CUartDriver::openDriver(void) {
    LOGD("openHardware: enter\n");
    m_uartfd = openUart();
    if (m_uartfd > 0) {
        LOGD("openUart: setUartSpeed...\n");
        setUartSpeed(m_uartfd, UART_BAUDRATE);
        if (setUartParity(m_uartfd, UART_DATABITS, UART_STOPBITS, UART_PARITY) == false) {
            LOGD("openUart: Set Parity Error\n");
            return false;
        }
        LOGD("openUart: Success\n");
        return true;
    } else {
        LOGD("openHardware: false\n");
        return false;
    }
}

void CUartDriver::closeDriver(void) {
    if (m_uartfd > 0) {
        LOGD("关闭串口");
        close(m_uartfd);
    }
}

int CUartDriver::sendData(const void *databuf, int length) {
    if (m_uartfd > 0 && databuf != NULL && length>0){
        LOGD("writeBytes: Writing...");
        ssize_t nwrite = write(m_uartfd, databuf, length);
        return nwrite;
    } else {
        LOGE("writeBytes: Error");
        return 0;
    }
}

int CUartDriver::receiveData(void *databuf, int length) {

    if(databuf!=NULL && length <= UART_RPCLENGTH)
    {
        long nbytes = 0;
        if ((nbytes = read(m_uartfd, databuf, length)) > 0) {
            LOGD("receiveThread: read %ld data...", nbytes);
            return nbytes;
        }
    }
    return 0;
}

bool CUartDriver::setParamater(const char* cmd, void* arg, int length)
{
    LOGD("setParamater start...\n");
    if(strcmp(cmd, SET_BAUD) == 0 && arg != NULL && length == sizeof(int))
    {
        int baudrate = *((int*)arg);
        setUartSpeed(m_uartfd, baudrate);
        if (setUartParity(m_uartfd, UART_DATABITS, UART_STOPBITS, UART_PARITY) == false) {
            LOGD("openUart: Set Parity Error\n");
            return false;
        } else
        {
            LOGD("Set Baud %d OK", baudrate);
            return true;
        }
    }
    LOGD("setParamater end\n");
    return false;
}

bool CUartDriver::openUart(void) {
    LOGD("openUart: Start...\n");
    m_uartfd = open(UART_DEVICE, O_RDWR);
    if (m_uartfd < 0) {
        LOGE("Open Failed %d", m_uartfd);
        return false;
    }
    return true;
}

///////////////////////////////////////////////////////////////////
/**
*@brief  设置串口通信速率
*@param  fd     类型 int  打开串口的文件句柄
*@param  speed  类型 int  串口速度
*@return  void
*/
int speed_arr[] = {B115200, B38400, B19200, B9600, B4800, B2400, B1200, B300,
                   B115200, B38400, B19200, B9600, B4800, B2400, B1200, B300,};
int name_arr[] = {115200, 38400, 19200, 9600, 4800, 2400, 1200, 300,
                  115200, 38400, 19200, 9600, 4800, 2400, 1200, 300,};

bool CUartDriver::setUartSpeed(int fd, int speed) {
    int i;
    long status;
    struct termios Opt;
    tcgetattr(fd, &Opt);
    for (i = 0; i < sizeof(speed_arr) / sizeof(int); i++) {
        if (speed == name_arr[i]) {
            tcflush(fd, TCIOFLUSH);
            cfsetispeed(&Opt, speed_arr[i]);
            cfsetospeed(&Opt, speed_arr[i]);
            status = tcsetattr(fd, TCSANOW, &Opt);
            if (status != 0) {
                perror("tcsetattr fd1");
                return false;
            }
            tcflush(fd, TCIOFLUSH);
        }
    }
    return true;
}
///////////////////////////////////////////////////////////////////
/**
*@brief   设置串口数据位，停止位和效验位
*@param  fd     类型  int  打开的串口文件句柄
*@param  databits 类型  int 数据位   取值 为 7 或者8
*@param  stopbits 类型  int 停止位   取值为 1 或者2
*@param  parity  类型  int  效验类型 取值为N,E,O,,S
*/
bool CUartDriver::setUartParity(int fd, int databits, int stopbits, int parity) {
    struct termios options;
    if (tcgetattr(fd, &options) != 0) {
        perror("SetupSerial 1");
        return false;
    }
    options.c_cflag &= ~CSIZE;
    switch (databits) /*设置数据位数*/
    {
        case 7:
            options.c_cflag |= CS7;
            break;
        case 8:
            options.c_cflag |= CS8;
            break;
        default:
            fprintf(stderr, "Unsupported data size\n");
            return false;
    }
    switch (parity) {
        case 'n':
        case 'N':
            options.c_cflag &= ~PARENB;   /* Clear parity enable */
            options.c_iflag &= ~INPCK;     /* Enable parity checking */
            break;
        case 'o':
        case 'O':
            options.c_cflag |= (PARODD | PARENB); /* 设置为奇效验*/
            options.c_iflag |= INPCK;             /* Disnable parity checking */
            break;
        case 'e':
        case 'E':
            options.c_cflag |= PARENB;     /* Enable parity */
            options.c_cflag &= ~PARODD;   /* 转换为偶效验*/
            options.c_iflag |= INPCK;       /* Disnable parity checking */
            break;
        case 'S':
        case 's':  /*as no parity*/
            options.c_cflag &= ~PARENB;
            options.c_cflag &= ~CSTOPB;
            break;
        default:
            fprintf(stderr, "Unsupported parity\n");
            return false;
    }
    /* 设置停止位*/
    switch (stopbits) {
        case 1:
            options.c_cflag &= ~CSTOPB;
            break;
        case 2:
            options.c_cflag |= CSTOPB;
            break;
        default:
            fprintf(stderr, "Unsupported stop bits\n");
            return false;
    }
    /* Set input parity option */
    if (parity != 'n')
        options.c_iflag |= INPCK;

    /*bug modify ref https://www.cnblogs.com/zgq0/p/8078505.html*/
    LOGD("setUartParity: Set options OK\n");
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag &= ~OPOST;
    options.c_cflag |= (CLOCAL | CREAD);//添加的
    options.c_oflag &= ~(ONLCR | OCRNL); //添加的
    options.c_iflag &= ~(ICRNL | INLCR);//添加的
    options.c_iflag &= ~(IXON | IXOFF | IXANY); //添加的

    tcflush(fd, TCIFLUSH);
    options.c_cc[VTIME] = 150; /* 设置超时15 seconds*/
    options.c_cc[VMIN] = 0; /* Update the options and do it NOW */
    if (tcsetattr(fd, TCSANOW, &options) != 0) {
        perror("SetupSerial 3");
        return false;
    }

    return true;
}
