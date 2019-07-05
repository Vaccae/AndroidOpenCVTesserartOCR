package dem.vac.tesseractocr;

import android.Manifest;
import android.content.pm.PackageManager;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.RelativeLayout;
import android.widget.TextView;

import javax.security.auth.login.LoginException;

public class MainActivity extends AppCompatActivity implements TesseratCallBack {


    private TextView tvshowtext;
    private VaccaeSurfaceView surfaceView;
    private RelativeLayout surfaceviewlayout;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        requestPermission();

        surfaceviewlayout=findViewById(R.id.surfaceviewlayout);

        surfaceView=new VaccaeSurfaceView(MainActivity.this);
        surfaceviewlayout.addView(surfaceView);

        // Example of a call to a native method
        tvshowtext=findViewById(R.id.tvshowtext);
        tvshowtext.setText("");
    }


    void requestPermission(){
        final int REQUEST_CODE = 1;
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, new String[]{
                            Manifest.permission.CAMERA, Manifest.permission.WRITE_EXTERNAL_STORAGE,
                            Manifest.permission.READ_EXTERNAL_STORAGE},
                    REQUEST_CODE);
        }
    }

    @Override
    public void CallBackOver(String... msg) {
        surfaceView.Setshowtext(msg[0]);
    }
}
