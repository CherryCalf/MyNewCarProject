//
// Created by hegf on 2020/4/10.
//

#include <zconf.h>
#include <cstdlib>
#include "../inc/CRPCManager.h"
#include "../inc/CUartDriver.h"

int sem_timedwait_millsecs(sem_t *sem, long msecs) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    long secs = msecs / 1000;
    msecs = msecs % 1000;

    long add = 0;
    msecs = msecs * 1000 * 1000 + ts.tv_nsec;
    add = msecs / (1000 * 1000 * 1000);
    ts.tv_sec += (add + secs);
    ts.tv_nsec = msecs % (1000 * 1000 * 1000);

    return sem_timedwait(sem, &ts);
}

CRPCManager *CRPCManager::m_instance = NULL;
IComDriver *CRPCManager::m_comDriver = NULL;

RPC_CALLBACK_st *CRPCManager::m_callback = NULL;
sem_t CRPCManager::recvok_sem;
sem_t CRPCManager::rpcack_sem;

bool CRPCManager::ackret = false;

pthread_mutex_t CRPCManager::mutex = PTHREAD_MUTEX_INITIALIZER;
bool CRPCManager::bExit = false;
pthread_t CRPCManager::thread_rpc;

bool CRPCManager::initThread() {
    LOGD("initThread");
    LOGD("等待设置监听器信号");
    //等待设置监听器信号
    sem_wait(&(CRPCManager::recvok_sem));

    if (m_state == RPC_DEINIT) {
        if (m_comDriver->openDriver() != true) {
            m_comDriver->closeDriver();
            usleep(1000);
            if (m_comDriver->openDriver() != true) {
                LOGE("串口驱动未正确开启");
                return false;
            }
        }

        if (NULL != CRPCManager::m_callback->preCallback) {
            //通信前初始化
            CRPCManager::m_callback->preCallback();
            //初始化状态
            m_state = RPC_INIT;
            LOGD("RPC_INIT");
            //初始化线程控制Flag
            bExit = false;
            return true;
        } else {
            return false;
        }
    } else {
        LOGE("当前状态串口忙");
        return false;
    }

}

void CRPCManager::Run() {
    //srand(time(NULL));
    LOGD("Callback OK start thread!");
    uint8_t testdata[16];
    while (!CRPCManager::bExit) {
        //int8_t testdata[] = {(int8_t)(rand()%255),(int8_t)(rand()%255), (int8_t)(rand()%255), (int8_t)(rand()%255), (int8_t)(rand()%255)};
        if (NULL != CRPCManager::m_callback->pRpcCallback) {
            //读串口数据
            if (m_comDriver != NULL) {
                memset(testdata, 0, sizeof(testdata));
                int nwrite = m_comDriver->receiveData(testdata, sizeof(testdata));
                if (nwrite > 0) {
                    CRPCManager::m_callback->pRpcCallback(testdata, nwrite);

                    LOGD("CRPCManager %d %d", testdata[0]&0xFF, 0xEE);
                    LOGD("CRPCManager %d %d", testdata[1], 0x10);
                    LOGD("CRPCManager %d %d", testdata[2], 0x00);
                    LOGD("CRPCManager %d %d", testdata[3], 0x01);
                    LOGD("CRPCManager %d %d", testdata[4], 0x00);
                    LOGD("CRPCManager %d %d", testdata[5], 0x02);
                    LOGD("CRPCManager %d %d", testdata[6], 0x01);
                    if ((testdata[0]&0xFF) == 0xEE &&
                        testdata[1] == 0x10 &&
                        testdata[2] == 0x00 &&
                        testdata[3] == 0x01 &&
                        testdata[4] == 0x00 &&
                        testdata[5] == 0x02 &&
                        testdata[6] == 0x01)
                    {
                        pthread_mutex_lock(&mutex);
                        CRPCManager::ackret = 1;
                        LOGD("CRPCManager dackret %d",CRPCManager::ackret);

                        sem_post(&(CRPCManager::rpcack_sem));
                        pthread_mutex_unlock(&mutex);
                    }
                    else if ((testdata[0]&0xFF) == 0xEE &&
                               testdata[1] == 0x10 &&
                               testdata[2] == 0x00 &&
                               testdata[3] == 0x01 &&
                               testdata[4] == 0x00 &&
                               testdata[5] == 0x02 &&
                               testdata[6] == 0x00)
                    {
                        pthread_mutex_lock(&mutex);
                        CRPCManager::ackret = 0;
                        sem_post(&(CRPCManager::rpcack_sem));
                        pthread_mutex_unlock(&mutex);
                    }

                } else {
                    LOGE("Recevied Data Error");
                }
            } else {
                LOGE("驱动未打开");
                break;
            }
        }
        //usleep(10000);
    }

}

void CRPCManager::exitThread() {
    if (NULL != CRPCManager::m_callback->afterCallback) {
        //推出前销毁
        CRPCManager::m_callback->afterCallback();
        LOGD("线程退出");
        if (m_state != RPC_DEINIT) {
            m_state = RPC_DEINIT;
            LOGD("RPC_DEINIT");
        }
    }
}

void CRPCManager::exitRun() {
    bExit = true;
    if (m_state != RPC_DEINIT) {
        m_state = RPC_CLOSING;
        LOGD("RPC_CLOSING...");
    }
    m_comDriver->closeDriver();

}

CRPCManager *CRPCManager::getInstance() {
    if (NULL == m_instance) {
        pthread_mutex_lock(&mutex);
        if (NULL == m_instance) {
            m_instance = new CRPCManager(RPC_DEINIT);
            m_comDriver = new CUartDriver();
            //信号的初始化
            sem_init(&recvok_sem, 0, 0);
            sem_init(&rpcack_sem, 0, 0);
        }
        pthread_mutex_unlock(&mutex);
    }
    return m_instance;
}

int CRPCManager::rpcSendCmd(int8_t *rpccmd, int length) {
    //向串口发送数据
    for (int i = length - 1; i >= 0; i--) {
        LOGD("%02x, ", rpccmd[i]);
    }
    LOGD("\n");

    if (m_state == RPC_INIT && m_comDriver != NULL) {
        m_comDriver->sendData(rpccmd, length);
        sem_timedwait_millsecs(&(CRPCManager::rpcack_sem), 50);
        pthread_mutex_lock(&mutex);
        bool ret = ackret;
        LOGD("ret %d, ", ret);
        pthread_mutex_unlock(&mutex);
        return ret;
    }
    return false;
}

void CRPCManager::rpcSetOnCallback(RPC_CALLBACK_st *callback) {
    if (NULL == callback) {
        return;
    }

    //注册callback
    m_callback = callback;
    //发送Callback注册成功信号
    LOGD("发送Callback注册成功信号");
    sem_post(&recvok_sem);
}

CRPCManager::CRPCManager(RPC_STATE_en mState) : m_state(mState) {

}
