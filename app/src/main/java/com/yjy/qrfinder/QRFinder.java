package com.yjy.qrfinder;

import android.graphics.Bitmap;

/**
 * <pre>
 *     @author : yjy
 *     @e-mail : yujunyu12@gmail.com
 *     @date   : 2019/10/22
 *     desc   :
 *     github:yjy239@gitub.com
 * </pre>
 */
public class QRFinder {
    static {
        System.loadLibrary("native-lib");
        System.loadLibrary("opencv_java3");
    }


    public static native Bitmap findQR(Bitmap bitmap);
}
