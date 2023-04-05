package com.smartspirt.carradioproject;

import android.nfc.Tag;
import android.util.Log;

import static com.smartspirt.carradioproject.ScreenUtils.TAG;

public class CarStateManager {

    private OnDoorStateListner m_onDoorStateListener = null;

    public interface OnDoorStateListner {
        //通过RPC的通信层获取并返回整形的频率数据
        void onDoorStateCallback(int doorstate);
    }

    public CarStateManager(IRPCCommunication m_rpcInstance) {
        this.m_rpcInstance = m_rpcInstance;
    }

    //依赖于一个RPC通信的接口对象
    private IRPCCommunication m_rpcInstance;

    public void closeRPC() {
        m_rpcInstance.closeComDevice();
    }

    public void openRPC() {
        m_rpcInstance.openComDevice();
    }

    //实时获取车门开关的监听接口
    public void setOnDoorStateListner(final OnDoorStateListner l) {
        m_onDoorStateListener = l;
        //通过通信层 进行RPC返回数据的Callback监控 和帅选
        m_rpcInstance.setOnRPCBackDataListener(new OnRPCBackDataListener() {
            @Override
            public void onRPCBackData(byte[] backdata) {
                //通过RPC协议工具类进行车辆状态数据的筛选
                Log.d(ScreenUtils.TAG, ScreenUtils.byteArrayToHexStr(backdata));

                int doorstate = RPCProtocolUtils.getDoorState(backdata);
                if (l != null) {
                    l.onDoorStateCallback(doorstate);
                }
            }
        });
    }

}
