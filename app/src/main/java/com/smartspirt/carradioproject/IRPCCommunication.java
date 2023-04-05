package com.smartspirt.carradioproject;

public interface IRPCCommunication {
    //发送数据
    public int sendCmdData(byte[] cmddata);
    //接收数据（Callback）
    public void setOnRPCBackDataListener(OnRPCBackDataListener l);
    //通信打开
    public int openComDevice();
    //通信关闭
    public void closeComDevice();
}
