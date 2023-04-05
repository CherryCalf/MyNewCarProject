package com.smartspirt.carradioproject;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.widget.RadioButton;
import android.widget.RatingBar;
import android.widget.SeekBar;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    //通过串口的RPC通信的单例，创建对象
    private CarStateManager csm = new CarStateManager(UartRPCCommunication.getInstance());
    private AirConManager am = new AirConManager(UartRPCCommunication.getInstance());
    private TextView testcount;
    private TextView okcount;
    private TextView okpersent;
    private TextView tv_doorstate;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        //打开RPC通信
        csm.openRPC();
        am.openAirCon();

        testcount = findViewById(R.id.testcount);
        okcount = findViewById(R.id.okcount);
        okpersent = findViewById(R.id.okpersent);
        tv_doorstate = findViewById(R.id.doorstate);

        //设置车门状态监听
        csm.setOnDoorStateListner(new CarStateManager.OnDoorStateListner() {
            @Override
            public void onDoorStateCallback(final int doorstate) {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if(doorstate == 0){
                            tv_doorstate.setText("车门关");
                        }
                        else
                        {
                            tv_doorstate.setText("车门开");
                        }
                    }
                });
            }
        });


    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        csm.closeRPC();
        am.closeAirCon();
    }

    public void onRPCTest(View view) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                int ok = 0;
                for(int i=0; i<100; i++)
                {
                    try {
                        int ackret = am.sendTestRPC();
                        Log.d(ScreenUtils.TAG, String.valueOf(ackret));
                        final int finalI = i+1;
                        if(ackret == 1)
                        {
                            ok++;
                        }
                        final int finalOk = ok;
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                testcount.setText(String.valueOf(finalI));
                                okcount.setText(String.valueOf(finalOk));
                                okpersent.setText(String.valueOf(finalOk)+"%");
                            }
                        });
                        Thread.sleep(100);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }
        }).start();

    }

    public void onOpenAirAC(View view) {
        am.openAirAC();
    }

    public void onCloseAirAC(View view) {
        am.closeAirAC();
    }

    public void onStartAIControl(View view) {
        startActivity(new Intent(this, MainActivity2.class));
    }
}
