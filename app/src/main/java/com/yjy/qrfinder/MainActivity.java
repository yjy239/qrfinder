package com.yjy.qrfinder;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.ImageView;

import com.yjy.qrfind.QRFinder;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        ImageView imageView = findViewById(R.id.sample_img);
        Bitmap bitmap = BitmapFactory.decodeResource(getResources(),R.mipmap.test3);


        imageView.setImageBitmap(QRFinder.findQR(bitmap));


    }


}
