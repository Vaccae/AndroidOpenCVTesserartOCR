package dem.vac.tesseractocr;

import android.content.Context;
import android.graphics.Bitmap;
import android.os.AsyncTask;
import android.os.Environment;
import android.util.Log;

import com.googlecode.tesseract.android.TessBaseAPI;

import java.io.File;

/**
 * 作者：Vaccae
 * 邮箱：3657447@qq.com
 * 创建时间：2019-06-21 14:05
 * 功能模块说明：
 */
public class VaccaeTesserat extends AsyncTask<Bitmap, String, String> {

    private Context mContext;
    private TesseratCallBack mCallBack;

    public VaccaeTesserat(Context context) {
        mContext=context;
        mCallBack=(TesseratCallBack)mContext;
    }


    @Override
    protected String doInBackground(Bitmap... bitmaps) {
        TessBaseAPI tessAPI=null;
        try {

            StringBuilder sb=new StringBuilder();
            // 核心预设置代码
            tessAPI=new TessBaseAPI();
            //如果Android的版本大于23，路径取根目录下的tesserart，小于的话是
            //在mnt/sdcard下面
            String path=Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator + "tesserart";
            tessAPI.setDebug(true);
            tessAPI.init(path , "eng");
//            tessAPI.setPageSegMode(TessBaseAPI.PageSegMode.PSM_AUTO);
            tessAPI.setVariable(TessBaseAPI.VAR_CHAR_WHITELIST, "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
            tessAPI.setVariable(TessBaseAPI.VAR_CHAR_BLACKLIST, "!@#$%^&*()_+=-[]}{;:'\"\\|~`,./<>?");

            //第一张为原图不取
            for (int i=1; i < bitmaps.length; i++) {
                tessAPI.setImage(bitmaps[i]);
                // 获取并显示识别结果
                sb.append(tessAPI.getUTF8Text());
            }

            mCallBack.CallBackOver(sb.toString());
        } catch (Exception e) {
            Log.e("Tess", e.getMessage());
            mCallBack.CallBackOver(e.getMessage());
        } finally {
            tessAPI.clear();
            tessAPI.end();
        }

        return null;
    }
}
