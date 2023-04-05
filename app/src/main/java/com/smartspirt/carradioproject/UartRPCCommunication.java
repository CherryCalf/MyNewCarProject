package com.smartspirt.carradioproject;

import android.util.Log;

import static java.lang.Thread.sleep;

//RPC通信类 需要设计为单例模式
public class UartRPCCommunication implements IRPCCommunication {

    private static final String TAG = "CAR_RADIO";

    private volatile static UartRPCCommunication m_instance;
    private OnRPCBackDataListener m_rpcbackListener = null;

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("nativerpc");
    }

    private UartRPCCommunication() {
    }
    //取得单例对象
    public static UartRPCCommunication getInstance() {
        if (m_instance == null) {
            //线程锁
            synchronized (UartRPCCommunication.class) {
                if (m_instance == null) {
                    m_instance = new UartRPCCommunication();
                }
            }
        }
        return m_instance;
    }


    @Override
    public int sendCmdData(byte[] cmddata) {
        //向串口发送命令的逻辑
        //测试代码
        for(int i=cmddata.length-1 ;i>=0; i--)
        {
            Log.d(ScreenUtils.TAG, ""+cmddata[i]);
        }
        int ret = sendCmd(cmddata);

        return ret;
    }

    @Override
    public void setOnRPCBackDataListener(OnRPCBackDataListener l) {
        m_rpcbackListener = l;
        //真正要做的 将RPCBackDataListener设置为JNI，作为C++的回调进行监控数据返回
        setRpcListener(m_rpcbackListener);
    }

    @Override
    public int openComDevice() {
        //串口打开的逻辑
        openRPCDevice();
        return 0;
    }

    @Override
    public void closeComDevice() {
        //串口关闭的逻辑
        closeRPCDevice();
    }

    private native int sendCmd(byte[] rpccmd);
    private native void openRPCDevice();
    private native void closeRPCDevice();
    private native void setRpcListener(OnRPCBackDataListener listener);

}
