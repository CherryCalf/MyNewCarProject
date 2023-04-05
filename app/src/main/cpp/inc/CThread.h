//
// Created by hegf on 2020/4/20.
//

#ifndef MYJNIDEMO_CTHREAD_H
#define MYJNIDEMO_CTHREAD_H

#include <pthread.h>
#include "CPRClog.h"

class CThread {
public:
    CThread(){};
    virtual ~CThread(){};
public:
    virtual bool initThread(){ return true;};
    virtual void Run(){
        LOGD("CThread Base Run");
    };
    virtual void exitThread(){
        if(m_thread > 0) {
            pthread_exit(&m_thread);
            m_thread = 0;
        }
    };

public:
    bool createThreadAndRun(){
        if(pthread_create(&m_thread, NULL, _ThreadEntry, this)==0)
        {
            return true;
        }
        else
        {
            return false;
        }
    };
public:
    static void* _ThreadEntry(void* pParam)
    {
        CThread* pThread = static_cast<CThread *>(pParam);
        if(NULL == pThread)
        {
            return NULL;
        }
        else
        {
            if(pThread->initThread())
            {
                pThread->Run();
                pThread->exitThread();
            }
            return NULL;
        }
    }
private:
    pthread_t m_thread;
};


#endif //MYJNIDEMO_CTHREAD_H
