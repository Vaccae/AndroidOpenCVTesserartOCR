package dem.vac.tesseractocr;

import android.graphics.Bitmap;

import java.util.List;

/**
 * 作者：Vaccae
 * 邮箱：3657447@qq.com
 * 创建时间：2019-06-19 11:29
 * 功能模块说明：
 */
    public class VaccaeOpenCVJNI {

    static {
        System.loadLibrary("native-lib");
    }

    public native static List<Bitmap> getCameraframebitbmp(Bitmap bmp,String text);

    public native static void loadcascade(String filepath);
}
