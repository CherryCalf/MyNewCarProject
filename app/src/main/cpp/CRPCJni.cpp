#include <jni.h>
#include <string>
#include "../inc/CRPCManager.h"
#include "../inc/CPRClog.h"

jobject gListener = NULL;
JavaVM *g_jvm = NULL;
JNIEnv *g_env = NULL;

int isNeedDetach = JNI_FALSE;

jclass listenerClass;
jmethodID callbackid;

void precallback()
{
    //通过JVM虚拟机获取JNIEnv环境指针
    int getEnvStat = g_jvm->GetEnv((void **) &g_env,JNI_VERSION_1_6);
    if (getEnvStat == JNI_EDETACHED) {
        //如果没有， 主动附加到jvm环境中，获取到env
        if (g_jvm->AttachCurrentThread(&g_env, NULL) != 0) {
            return;
        }
        isNeedDetach = JNI_TRUE;
    }
    listenerClass = g_env->GetObjectClass(gListener);
    callbackid = g_env->GetMethodID(listenerClass, "onRPCBackData", "([B)V");
}

void aftercallback()
{
    //从jvm环境中注销线程
    if(isNeedDetach) {
        g_jvm->DetachCurrentThread();
    }

    g_jvm = NULL;
    g_env = NULL;
}

//RPC回调数据Callback
void reccallback(uint8_t* rpcdata, int length)
{
    if(NULL == rpcdata || length == 0)
    {
        return;
    }

    if(NULL == gListener || g_jvm == NULL)
    {
        return;
    }

    //回调JAVA接口
    jbyteArray jdata = g_env->NewByteArray(length);
    g_env->SetByteArrayRegion(jdata, 0, length, (int8_t*)rpcdata);
    g_env->CallVoidMethod(gListener, callbackid, jdata);
    g_env->DeleteLocalRef(jdata);

}

extern "C"
JNIEXPORT int JNICALL
Java_com_smartspirt_carradioproject_UartRPCCommunication_sendCmd(JNIEnv *env, jobject instance, jbyteArray rpccmd_) {

    //jbyteArray转换为C++数组
    jbyte *rpccmd = env->GetByteArrayElements(rpccmd_, NULL);
    //获取RPC命令的数组的长度
    int len = env->GetArrayLength(rpccmd_);
    //调用通信模块RPCManager的接口发送RPC命令数据
    int ackret = CRPCManager::getInstance()->rpcSendCmd(rpccmd, len);
    //释放C++数组
    env->ReleaseByteArrayElements(rpccmd_, rpccmd, 0);
    return ackret;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_smartspirt_carradioproject_UartRPCCommunication_openRPCDevice(JNIEnv *env, jobject instance) {
    //获取JVM虚拟机，以备Callback中使用JVM获取JNIEnv环境参数
    env->GetJavaVM(&g_jvm);
    //初始化RPC
    CRPCManager::getInstance()->createThreadAndRun();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_smartspirt_carradioproject_UartRPCCommunication_closeRPCDevice(JNIEnv *env, jobject instance) {
    //销毁RPC
    CRPCManager::getInstance()->exitRun();

    //销毁Java注册过来的监听对象
    if(gListener != NULL)
    {
        env->DeleteGlobalRef(gListener);
        gListener = NULL;
    }
}

//设置RPCManager的回调
RPC_CALLBACK_st callback = {
        precallback,
        reccallback,
        aftercallback
};

extern "C"
JNIEXPORT void JNICALL
Java_com_smartspirt_carradioproject_UartRPCCommunication_setRpcListener(JNIEnv *env, jobject instance,
                                                     jobject listener) {
    //保存HMI传递过来的监听对象为全局引用
    gListener = env->NewGlobalRef(listener);
    //注册给通信模块RPCManager回调函数（RPC_CALLBACK_st callback）
    CRPCManager::getInstance()->rpcSetOnCallback(&callback);

}
