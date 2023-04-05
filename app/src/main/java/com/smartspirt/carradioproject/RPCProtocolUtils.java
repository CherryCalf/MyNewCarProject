package com.smartspirt.carradioproject;

import android.util.Log;

import java.net.URLEncoder;

public class RPCProtocolUtils {
    private static final int RPC_LENGTH = 16;
    public static byte[] rpcdata = new byte[RPC_LENGTH];

    private static void clearRPCData() {
        for (int i = 0; i < RPC_LENGTH; i++) {
            rpcdata[i] = 0x00;
        }
    }

    //从rpc原始数据中判断并获取车门开关状态
    public static int getDoorState(byte[] backdata) {
        if ((backdata[0] & 0xFF) == 0xEE &&
                backdata[1] == 0x10 &&
                backdata[2] == 0x00 &&
                backdata[3] == 0x02 &&
                backdata[4] == 0x00 &&
                backdata[5] == 0x01 &&
                backdata[6] == 0x00) {
            return 0;
        } else if ((backdata[0] & 0xFF) == 0xEE &&
                backdata[1] == 0x10 &&
                backdata[2] == 0x00 &&
                backdata[3] == 0x02 &&
                backdata[4] == 0x00 &&
                backdata[5] == 0x01 &&
                backdata[6] == 0x01) {
            return 1;
        }
        return 0;
    }

    public static byte[] makeAirACONCmd(boolean on) {
        clearRPCData();
        setRPCHeaderAndLength();
        setAirConModelID();
        setAirConACFunID();
        if (on) {
            rpcdata[9] = 0x01;
        } else {
            rpcdata[9] = 0x00;
        }
        setCheckSum();
        return rpcdata;
    }

    private static void setAirConModelID() {
        rpcdata[13] = 0x00;
        rpcdata[12] = 0x01;
    }

    public static byte[] makeTestRPCCmdData() {
        clearRPCData();
        setRPCHeaderAndLength();
        setAirConModelID();
        setAirConACFunID();
        rpcdata[9] = 0x01;
        setCheckSum();
        return rpcdata;
    }

    private static void setCheckSum() {
        int checksum = 0;
        for (int i = 15; i > 1; i--) {
            checksum += rpcdata[i] & 0xFF;
        }
        rpcdata[1] = (byte) (checksum / 256);
        rpcdata[0] = (byte) (checksum % 256);
    }

    private static void setAirConACFunID() {
        rpcdata[11] = 0x00;
        rpcdata[10] = 0x02;
    }

    private static void setRPCHeaderAndLength() {
        rpcdata[15] = (byte) 0xFE;
        rpcdata[14] = 0x10;
    }
}
