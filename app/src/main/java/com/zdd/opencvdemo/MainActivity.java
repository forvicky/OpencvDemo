package com.zdd.opencvdemo;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Point;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;

import com.zdd.opencvdemo.utils.FileUtil;

import java.io.File;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {
    private ImageView imageView;
    private static final String FILE_NAME="lbpcascade_frontalface.xml";

    static {//加载so库
        System.loadLibrary("native-lib");
    }

    private File mCascade;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        imageView = findViewById(R.id.imageView);
        findViewById(R.id.show).setOnClickListener(this);
        findViewById(R.id.process).setOnClickListener(this);

        mCascade=new File(getDir("cascade", Context.MODE_PRIVATE),FILE_NAME);
        FileUtil.copyAssetsToFile(this,FILE_NAME,mCascade.getAbsolutePath());
    }

    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.show) {
            Bitmap bitmap = BitmapFactory.decodeResource(getResources(), R.drawable.test8);
            imageView.setImageBitmap(bitmap);
        } else {
            Bitmap bitmap = BitmapFactory.decodeResource(getResources(), R.drawable.test8);
            Bitmap bitmap2 = BitmapFactory.decodeResource(getResources(), R.drawable.test2);
//            imageView.setImageBitmap(cvtBitmap(bitmap,Bitmap.Config.ARGB_8888));


//            imageView.setImageBitmap(matchBitmap(bitmap,bitmap2,Bitmap.Config.ARGB_8888,5));

//            imageView.setImageBitmap(txtAreaBitmap(bitmap,Bitmap.Config.ARGB_8888));

            int size=faceDetector(bitmap,Bitmap.Config.ARGB_8888,mCascade.getAbsolutePath());
            imageView.setImageBitmap(bitmap);
            Log.d("zddHttp","size="+size);
        }
    }

    //图片灰度化
    public native Bitmap cvtBitmap(Bitmap bitmap, Bitmap.Config argb8888);

    //图片匹配，match采用5效果最好
    public native Bitmap matchBitmap(Bitmap bigBitmap,Bitmap smallBitmap, Bitmap.Config argb8888,int match);

    //图片提取文字区域,只能识别出简单的黑白图片
    public native Bitmap txtAreaBitmap(Bitmap bitmap, Bitmap.Config argb8888);

    public static native int faceDetector(Bitmap bitmap, Bitmap.Config argb8888, String path);

}