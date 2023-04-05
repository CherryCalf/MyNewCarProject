//
// Created by hegf on 2020/4/10.
//

#ifndef MYJNIDEMO_CRPCMANAGER_H
#define MYJNIDEMO_CRPCMANAGER_H

#include <semaphore.h>
#include "CThread.h"
#include "IComDriver.h"
#include "CPRClog.h"

typedef enum{
    RPC_INIT,
    RPC_DEINIT,
    RPC_CLOSING
}RPC_STATE_en;

typedef struct{
    void (*preCallback)();
    void (*pRpcCallback)(uint8_t* rpcdata, int length);
    void (*afterCallback)();
}RPC_CALLBACK_st;

class CRPCManager : public CThread{

public:
    CRPCManager(RPC_STATE_en mState);

    virtual bool initThread();
    virtual void Run();
    virtual void exitThread();

    void rpcInit();
    void rpcDeInit();
    void exitRun();
    int rpcSendCmd(int8_t* rpccmd, int length);
    void rpcSetOnCallback(RPC_CALLBACK_st* callback);
    static CRPCManager* getInstance();

    static sem_t recvok_sem;
    static sem_t rpcack_sem;
    static bool ackret;
public:
    static pthread_mutex_t mutex;
    static bool bExit;
    static pthread_t thread_rpc;
    static RPC_CALLBACK_st* m_callback;
private:
    static CRPCManager* m_instance;
    RPC_STATE_en m_state;
    static IComDriver* m_comDriver;

};


#endif //MYJNIDEMO_CRPCMANAGER_H
