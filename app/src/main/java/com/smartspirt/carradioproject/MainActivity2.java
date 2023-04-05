package com.smartspirt.carradioproject;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;

import com.baidu.aip.asrwakeup3.core.mini.ActivityMiniWakeUp;

public class MainActivity2 extends ActivityMiniWakeUp {

    //通过串口的RPC通信的单例，创建对象
    private CarStateManager csm = new CarStateManager(UartRPCCommunication.getInstance());
    private AirConManager am = new AirConManager(UartRPCCommunication.getInstance());

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        am.openAirCon();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        am.openAirCon();
    }

    @Override
    public void onEvent(String name, String params, byte[] data, int offset, int length) {
        String logTxt = "name: " + name;
        if (params != null && !params.isEmpty()) {
            logTxt += " ;params :" + params;
        } else if (data != null) {
            logTxt += " ;data length=" + data.length;
        }
        Log.d("AIControl", logTxt);
        if(logTxt.contains("打开空调"))
        {
            am.openAirAC();
        }
        else if(logTxt.contains("关闭空调"))
        {
            am.closeAirAC();
        }
    }
}
