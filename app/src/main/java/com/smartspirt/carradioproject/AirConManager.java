package com.smartspirt.carradioproject;

public class AirConManager {


    public AirConManager(IRPCCommunication m_rpcInstance) {
        this.m_rpcInstance = m_rpcInstance;
    }

    //依赖于一个RPC通信的接口对象
    private IRPCCommunication m_rpcInstance;

    public void closeAirCon(){
        m_rpcInstance.closeComDevice();
    }

    public  void openAirCon()
    {
        m_rpcInstance.openComDevice();
    }

    public int sendTestRPC() {
        int ackret = -1;
        ackret = m_rpcInstance. sendCmdData(RPCProtocolUtils.makeTestRPCCmdData());
        return ackret;
    }

    public void openAirAC() {
        //byte[] tmpdata = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,0x05,0x00,0x02,0x00};
        m_rpcInstance.sendCmdData(RPCProtocolUtils.makeAirACONCmd(true));
    }

    public void closeAirAC() {
        //byte[] tmpdata = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x05,0x00,0x02,0x00};
        m_rpcInstance.sendCmdData(RPCProtocolUtils.makeAirACONCmd(false));
    }
}
