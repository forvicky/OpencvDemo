package com.zdd.opencvdemo;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Point;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.ImageView;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {
    private ImageView imageView;

    static {//加载so库
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        imageView = findViewById(R.id.imageView);
        findViewById(R.id.show).setOnClickListener(this);
        findViewById(R.id.process).setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.show) {
            Bitmap bitmap = BitmapFactory.decodeResource(getResources(), R.drawable.test);
            imageView.setImageBitmap(bitmap);
        } else {
            Bitmap bitmap = BitmapFactory.decodeResource(getResources(), R.drawable.test);
            Bitmap bitmap2 = BitmapFactory.decodeResource(getResources(), R.drawable.test2);
//            imageView.setImageBitmap(cvtBitmap(bitmap,Bitmap.Config.ARGB_8888));


            imageView.setImageBitmap(matchBitmap(bitmap,bitmap2,Bitmap.Config.ARGB_8888,5));

        }
    }

    //图片灰度化
    public native Bitmap cvtBitmap(Bitmap bitmap, Bitmap.Config argb8888);

    //图片匹配，match采用5效果最好
    public native Bitmap matchBitmap(Bitmap bigBitmap,Bitmap smallBitmap, Bitmap.Config argb8888,int match);
}